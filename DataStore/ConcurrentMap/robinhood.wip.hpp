  bool    cleanDeletion(u32 i, u8 depth=0)     const
  {
    VerIdx curVi, nxtVi, preVi; VerIpd nxtVp; u32 nxtI, preI;

    clean_loop: while( (nxtVi=nxt(i,&nxtI)).idx < DELETED  && i!=m_sz-1)            // dupe_nxt stands for duplicate next, since we are duplicating the next VerIdx into the current (deleted) VerIdx slot
    {
      curVi = load_vi(i);
      if(curVi.idx==EMPTY){ return false; }

      nxtVp = ipd(nxtI, nxtVi.idx);
      if(nxtVp.version!=nxtVi.version || nxtVp.ipd==0){ 
        return true; 
      }else if( cmpex_vi(i, curVi, nxtVi) ){                                        // todo: should this be converted to an empty slot since it is the end of a span?  -  next slot's versions match and its VerIdx is in its ideal position, so we are done 
        delDupe(i);
        i = nxtIdx(i);
      }
    }

    if(i!=m_sz-1){
      if(nxtVi.idx == DELETED){                                                     // if the next index is deleted, try to clean the next index, then come back and try to delete this one again
        i=nxtIdx(i);
        if(depth<1){ cleanDeletion(i, depth+1); }                                   // could this recurse to the depth of the number of blocks?
        goto clean_loop;
      }else if(nxtVi.idx==EMPTY){
        delDupe(i);    
      }
    }else{}

    return true;
  }



/* swpOnMatch work in progress */
bool          swpOnMatch(u32 i, bool useLeft, u32 cmpLeft, u32 left, bool useRight, u32 cmpRight, u32 right)                 const                                // delete duplicate indices - l is left index, r is right index - will do something different depending on if the two slots are within 128 bit alignment or not
{
  if(i%2==0)
  {                                                                                 // both indices are within a 128 bit boundary, so the 128 bit atomic can (and must) be used
    i64     rgtSwp, lftSwp; 
    VerIdx  lftVi,  rgtVi;
    _u128   viDbl;
    _u128*  viDblAddr = (_u128*)&s_vis[i];                                          // find 128 bit offset address
    visFromAddr(viDblAddr, &lftVi, &rgtVi);
    //viDbl            = *viDblAddr;                                                // if it isn't the same, the atomic compare exchange will load it atomically
    do{
      u32 l = hi32(viDbl.lo);
      u32 r = lo32(viDbl.hi);
      bool  leftOk = ((useLeft  && l==cmpLeft)  || (!useLeft));
      bool rightOk = ((useRight && l==cmpRight) || (!useRight));
      if( leftOk && rightOk ){
        lftSwp = useLeft?  vi_i64(VerIdx(left,lftVi.version))   :  vi_i64(lftVi);
        rgtSwp = useLeft?  vi_i64(VerIdx(left,rgtVi.version))   :  vi_i64(rgtVi);
        //rgtSwp = useRight? vi_i64(make64(left,lo32(viDbl.lo)))  :  vi_i64( swp32(empty_vi().asInt) );
      }else if(l!=r || l>=DELETED){                                           // check if both the indices are the same and if they are, that they aren't both deleted or both empty 
        return false;
      }else{
        u64 lftSwp = swp32(viDbl.hi);
        lftDel     = *((i64*)&lftSwp);                                            // if both the indices are the same, make a new right side VerIdx with the idx set to DELETED
        rgtDel     = vi_i64( deleted_vi() );                                      // interpret the u64 bits directly as a signed 64 bit integer instead
      }
    }while( !compex128( (i64*)viDblAddr, rgtSwp, lftSwp, (i64*)&viDbl) );         // then compare and swap for a version with the new right side VerIdx
  }else
  {
    au64* idxDblAddr; u64 idxDbl, desired;
    u32* leftAddr = ((u32*)(&s_vis[i]))+1;                                        // if the two VerIdxs are not in a 128 bit boundary, then use a 64 bit compare and swap to set the right side index to DELETED
    idxDblAddr    = (au64*)leftAddr;                                              // increment the address by 4 bytes so that it lines up with the start of the two indices, then cast it to an atomic 64 bit unsigned integer for the compare and switch
    idxDbl        = idxDblAddr->load();
    do{
      u32  l = hi32(idxDbl);
      u32  r = lo32(idxDbl);
      if( (l==DELETED && r==EMPTY) || (l==EMPTY && r==DELETED) ){           // change the deleted key to empty if it is to the left of an empty slot and therefore at the end of a span
        desired = make64(EMPTY, EMPTY);          
      }else if(l!=r || l>=DELETED){
        return false; 
      }else{                                                                      // if the indices are the same then do the compare and swap
        desired = make64(l, DELETED);                                         // make the new 64 bit integer with the right index set to DELETED
      }
    }while( !idxDblAddr->compare_exchange_strong(idxDbl, desired) );              // looping here would essentially mean that the indices change but are still identical to each other
  }

  return true;
}


