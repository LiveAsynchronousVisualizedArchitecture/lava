


// 4. Use version from that struct to verify that each block is part of the list given by the ConcurrenHash entry.

/*
Robin Hood Hashing:
 |  The ConcurrentHash indices are organized so that when searching for a key by linear search, no encountered key has an ideal position distance lower than the key being searched for
 |  To do this concurrently, instead of looking for an empty or deleted slot in the ConcurrentHash array and simply inserting there (as with linear search), the VerIdx is put into a free slot and continuously swapped with its VerIdx to the left if the distance to its ideal position is larger.
 |  The concurrent swapping of two 64 bit structures is complicated by the fact that while 128 bit atomic compare and exchange instructions are present on most modern CPUs, they are only able to operate on 128 bit aligned memory. 
 |  While half the 128 bit pairs of two 64 bit structures could be swapped with this instruction, the other half can not.
 |  On x64 64 bit atomic compare and exchanges CAN be unaligned, meaning that if Index values are next to each other in memory, they can be compare-exchanged atomically or read atomically
 |  There are multiple insights and techniques working together that allow the concurrent swapping to happen:
 |  |  The Index of the VerIdx struct stored in the ConcurrentHash array is always unique, since it is the index into the block list.
 |  |  The Version of the VerIdx struct stored in the ConcurrentHash array is always unique if the BlockList's atomic version counter has not wrapped around after overflow
 |  |  Because both the Version AND the Index are unique, the same Version + Index combination cannot occur twice over the lifetime of the database unless the version has overflowed and wrapped around (and the same index into the BlockList ends up with the same Version)
 |  |  The 64 bit Version+Index structure (Version is 32 bits and Index is 32 bits) is flipped on even indices in the ConcurrentHash array. The memory is ordered as  |Idx Ver Ver Idx|Idx Ver Ver Idx| with '|' here representing 128 bit alignment boundaries
 |  |  The Index values, next to each other in an unaligned 64 bit configuration can be swapped atomically, though their indices can not be 
 |  del():
 |  |  Finds the matching key
 |  |  Makes sure the VerIdx matches in ConcurrentHash and atomically comapare exchanges it for a VerIdx with a DELETED_KEY index value
 |  |  Calls cleanupDeleted(i) with the current ConcurrentHash slot
 |  |  |  Checks the next slot to see if there is a VerIdx in it, and if so, check to see what its ideal position distance is
 |  |  |  If the ideal position distance is more than 0, it can be moved backwards, so duplicate it into the current slot with an atomic compare-exchange to make sure it is deleted
 |  |  |  Check the indices of the current and next slots to see if the indices are still identical and atomically compare-exchange the right index value to DELETED_KEY if they are
 |  |  |  The possibility exists that after duplication but before deletion of the duplicate, put() could swap the duplicate to the right (higher slot). Because of this, put will need to delete any indices that it finds that link to non-key block lists
 |  |  |  put() will also need to check for duplicate indices, since if they are bubble sorting a robin hood span, they could separate duplicate indices from being next to each other, but also sort them back together
 |  |  |  should get() also check for duplicates? - probably, since it would have tighter cleanup of the ConcurrentHash only at the expense of one or two extra atomic loads per read
*/





// q: is it possible to have only 128 bit aligned values, use the first as the value to read, the second as the value to write, and switch them? - how does that help the sorting problem?
// q: is it possible to go back to even / odd version numbers?  is it possible to simply update the version number on the block list if it still matches the old version number? - should new even and odd version numbers be gotten from the CncrLst each time?
// q: can the first and last slots both be 128 bit aligned and use two VerIdx values, creating a double buffer where the low 64 bits of the last slot and the high bits of the first slot are used normally, but the other 64 bits of each can be used as a buffer to swap in values from the other side
//    | read only needs to move forward and thus can use the extra slot as the first while skipping the actual first slot
//    | would put() and del() need to set a flag and spinlock on the first index so that they could modify both the extra slot and the first slot without any other threads modifying them  
// q: how can the last and first indices be handled so that the last index being deleted doesn't back up and result in all other indices before it ending up as DELETED because they can't know if they are at the end of a span?
// q: can't set a DELETED_KEY to EMPTY_KEY just because it is in front of an EMPTY_KEY, because you need to atomically know that the slot ahead is the end of a span - if the slot ahead IS the end of a span, the DELETED_KEY can be made into an EMPTY_KEY - does this imply that the last slot in m_vis will end up DELETED and infect slots behind it that can never be freed into EMPTY slots? - maybe when searching for a match, never treat the last index as the end of a span, and always set the last slot directly to EMPTY instead of DELETED
// q: make del() pay attention to finding an index whos version doesn't match, but how to handle that situation? just decrement readers? make sure to only care on the 128 bit alignment crossover - if del() is only looking for the key just as read is, then just decrement the readers? if the readers are already 0, then restart the search since it was already deleted
// q: have a bitfield of flags so that put() and del() will spinlock when swapping over the 128 bit boundary? - 
// q: have a third layer that is nothing but ordering? - have a bitfield that lets put() mark what boundaries it will swap? 

// todo: make read start from the ideal position when it finds a key on a removed block list and move back one position if it finds a duplicate entry
// todo: make put spin on returning to the CncrHsh and finding the same idx but a different version number - make put restart if 
// todo: make a bool flag (64 bytes aligned?) in CncrHsh that locks the first and last slots for swapping and makes any thread trying to swap the first and last slots spin and sleep(0) under contention

// robin hood hashing
// todo: do rm()/del() first and make deletion take care of holes in spans?
// todo: make put() insert at the first available slot then swap with one index lower if it has a IPD that is at least 2 less
//       | after swapping, set the block list's version to the new version of the index? - what if the found version is not what the old version was? leave it? 
// todo: align ConcurrentHash's lava vec on 128 bit boundary - need to make an extra 128bit value and 
// todo: -organize ConcurrentHash entry to have the index on the left side, version on the right side. 
//       Put hash in the middle and use the first two bits of the index as empty and deleted flags
//       empty : 1, deleted : 1, index : 35, hash : 35, version : 56 - total 128 bits, 34 billion entry limit 
// todo: test alignment
// todo: figure out deletion - look back at tbl with EMPTY and NONE types 
//       | even without robin hood hashing are 128 bit atomics needed to swap/bubble sort keys into the new free slot?
//       | when deleting, instead of swapping the deleted key, duplicate the next key to the previous key and so on until reaching either an empty slot, a key that is further from its optimal distance, or two of the same key in a row (indicating another thread is moving keys around)

// todo: change simdb to use init() that return error codes
// todo: make a 'waiting' flag or type for keys so that they can be rewritten and resized in place? - would mean that they could not be read from at any time like they can be now
// todo: make alloc look for multiple blocks then check the next block variable for its version and if the version has not changed, allocate all blocks at once?
// todo: make a resize/realloc function to change the size of a block list instead of destroying and creating all indices when updating a key? - would need a different putWeak, since the writing of the index needs  to be atomic and re-writing the currently used blocks would not work with concurrency
// todo: integrate realloc into put() function - will need to query to see if the key exists, delete it, then insert the reallocated version? - if done like this, does it disrupt for a moment a key existing, and if so, is this an insurmountable problem?

// q: do the Index numbers in VerIdx need a flag saying that they haven't been changed? - what if before a thread switches two indices, other threads delete both VerIdxs and create new VerIdxs with different ideal position distances, but the exact same Indexes? can storing the Index as an xor of the Version and the Index solve this or will there still be non unique combinations? Ths would also mean that the Index would not line up immediatly after the swap. Do the upper bits of a 64 bit version counter need to be used? If even/odd versions are used, does that alleviate the problem? Is there a way to do XOR swapping and be able to see the XOR in progress?
// q: what operations need ConcurrentHash Index values to have versions with them? 
//       | get() does not, since it can check the key value to verify the block list is what it wants and it can check the version number on the block list to make sure the blocks did not change while reading
//       | put() does, since it needs to know that the Index value it is going to change still points to the same data
//       | del() does, since it needs to know that the Index value it is going to change still points to the same data
// q: what if two version numbers are put into each block list starting entry? verA, verB, hash, and index - 128 bits
// q: if Index values are swapped atomically but their version numbers can not be known to be the same, does this require more flexibility on the part of the linear search? only stop when hitting IPD (ideal position distances) less than the IPD of the searched item more than once? 
// q: the 64 bit Index swap needs to check that both IPD are still the same after the swap?
// q: is it possible to know to go backwards when finding an element if the same index is seen with a different version on an adjacent VerIdx ?

// q: can cleanup be done by a ring buffer of block lists? would the ring buffer only need to be as long as the number of threads if each thread helps clear out the ring buffer after every free()? Probably not, because delayed deallocation would be useful only when there is a reader/ref count, which would mean many more reads than writes could stall the ability to free? But each thread can really only hold one reference at a time so maybe it would work? 
// q: if using a ring buffer, indices might be freed in between non-freed indices, in which case the pointer to beginning and end would not be able to shift, and therefore would need space for more indices than just the number of threads
// q: if using a ring buffer for frees, could a thread freeing the index then bubble sort backwards and move the begin pointer forwards until the begin and end pointers have only non freed indices between them 
// q: what is the benefit of a ring buffer? does it prevent the need for a spinlock anywhere?
// q: does the main program even need a key value store? can't it just use the block list indices directly?
// q: should maximum probe length be used here?
// q: should use a separate 'tombstone' value for a deleted value?
// q: should store hash to be able to know probe length and as a short circuit for false matches?
// q: can deleted entries be returned to empty entries on removal or some other operation somehow? if robinhood hashing means keeping the probe distance to only partial overlapping spans, does that open up the possibility?
//    - on removal, if the previous entry is EMPTY or if the previous entry has a probe span of 0, set the current entry to EMPTY
//      can actually loop backwards the (length of the span) - 1 (to the natural position + 1) to see if there are any overlapping spans?
//      on removal, loop backwards to robin hood hash any overlapping span
//      after robin hood hashing from the natural position set any DELETED keys after to EMPTY since they are after a span of 0 or after EMPTY 
//    - if the current entry is EMPTY, loop forward setting adjacent DELETED entries to EMPTY
// q: does robinhood hashing mean keeping a stack of moved values until the end of a robery chain is complete, then copying from the end of the chain to beggining? 
// q: could ISPC be compiled to C++ intrinsics to integrate into a one header file structure? - should be useful for hashing and comparison?
// q: can make a bitmap for filled hash indices if the bit is set after the KV is set, and the bit is unset before unsetting the KV
// q: can intrinsics be used for 128bit atomics?
// q: make ConcurrentHash be a 48 bit index and a 48 bit version
//    - pack them together and use unaligned atomic reads and writes?
//    - be able to modify an index,version,index triplet with a 128 bit atomic or a version,index,version triplet?  
// q: can some sort of stack or list or current outstanding version reads be used to make sure version numbers don't wrap around, therefore needing less bits for the version number?
// q: can a high bits version number and low bits version number be used to detect vesion number wrap arounds?  One 64 bit number while just the lower bits are stored?
// q: interleave reference counting / readers count on both hash and store?
//    - when reading from a ConcurrentHash value, increment the readers by 1, increment the block idx by 1, then decrement the ConcurrentHash
//    - does this destroy the ability to do robinhood hashing without spinlocking?
// q: on removal, follow DELETED entries forward until hitting a key entry, and if the removed entry was inside its probe span, bring the found entry up by copying, then remove the found entry, then loop forward, trying to set DELETED entries to EMPTY if they are after EMPTY or after the end of a probe span
// q: can part of the version be put on one side of a struct and part on the other side? maybe even interleave bits?
// q: just have a flag bit on each side? this would allow signaling that only one thread at a time is doing the removal cleanup and allow cleanup concurrently with insertion? 
//    1. flip bits in between two entries
//    ////2. consider the opposite bits to be 'changed' bits and flip them only if an insert sees either of the bits flipped
//    ///////3. if the removal process detects a 'changed' bit, ... 
//    //////4. is it needed if versions are involved, and even so would it work?
//    2. what is the problem? - need to be able to tell if previous entry is the same when changing the next entry 
//    3. keep index first and version second so that it is possible to loop forward and look at the previous version and the current index - if a high bit is used as a flag for empty, then the low bits of the version and the high bits of the index could be change atomically, meaning 32 bit precision for versions only in one brief moment 
//       !- if only the upper bit is needed, can the 64 bit atomic operation happen with 56 bits for the version and only 8 bits for the index?
//    4. then check the whole version and do some sort of error handling and recovery if the version is not the same - although 56 bits should be a year's worth of cpu cycles, so having 72 quadrillion allocations between a few instructions on another thread seems unlikely 
//    5. in theory a mutex could be used and the entire range of versions could be remapped and the master version atomic could be set to the new max+1
// q: is it possible to defragment and if so it is possible to do on removal?



bool     emptyIfAhead()                      const{}

bool        isSpanEnd(u32 i, u32 blkIdx)     const
{ 
  return (i!=m_sz-1 && load(i).idx==EMPTY) || ipd(i,blkIdx).ipd==0;                 // last slot is never treated as the end of a span since it cannot be manipulated atomically with the next slot (since the next slot is the first slot) - this is so that the rest of the table can be cleaned up
}

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


