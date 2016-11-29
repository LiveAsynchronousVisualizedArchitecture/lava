//str  key = db.getKeyStr(db.nxt());
//str  val(16, '\0');
//db.get(key.c_str(), (void*)val.data() );
//val.resize( strlen(val.c_str()) );

//str  key( db.len(db.nxt()), '\0');
//auto len = db.getKey(db.cur(), (void*)key.data(), (ui32)key.length());
//str  val(16, '\0');
//db.get(key, (void*)val.data() );
//val.resize( strlen(val.c_str()) );
//
//Println("len: ", len, 
//    " str len: ", 
//    key.length(), "  nxt key: [", key, 
//    "]  val: [", val,"] val len: ", val.length() );    

//auto idx = db.nxt();
//str key( db.len(idx), '\0');
//auto len = db.getKey(idx, (void*)key.data(), (ui32)key.length());

//auto idx2 = db.get((void*)kablam.data(), (ui32)kablam.length());
//Println("get \"kablam\": ", idx2);

//UnmapViewOfFile(mapmem);
//CloseHandle(fileHndl);

//DWORD                 dwMaximumSizeHigh,
//DWORD                 dwMaximumSizeLow,
//LPCTSTR               lpName
//);

//auto idx = db.get((void*)wat.data(), (ui32)wat.length());
//Println("get: ", idx);
//db.get(idx, (void*)clear.data() );

//struct keyval
//{
//  uint64_t  readers  :   4;
//  uint64_t      key  :  28;
//  uint64_t      val  :  28;
//};
//Println("kv size: ",   sizeof(ConcurrentHash::kv) );
//Println("kv size: ",   sizeof(keyval) );
//Println("ui64 size: ", sizeof(ui64) );

//Aui32*  key_addr  =  (Aui32*)(&m_keys.get()[i]);
//ui32   probedKey  =  atomic_load( key_addr );                                 
//atomic_store( (Aui32*)(&m_vals.get()[i]), val);

//ui32 i  =  0;
//for(; i < m_sz; ++i)

//copy( m_keys.get(), m_keys.get()+sz, EMPTY_KEY );
//copy( m_keys.get(), m_keys.get()+sz, EMPTY_KEY );

//m_keys.get()[i]  =  EMPTY_KEY;
//m_vals.get()[i]  =  EMPTY_KEY;

//~ConcurrentMap()
//{
//  //m_mut.unlock(); // if this is already unlocked, will this crash?
//}

//using Keys      =  std::vector<ui32>;
//using Values    =  std::vector<ui32>;

//ui32     put(ui32 key, ui32 val) const
//{
//  using namespace std;
//
//  ui32 empty_ref = EMPTY_KEY;
//  
//  for(ui32 i=0; true; ++i)
//  {
//    ui32 prevKey = m_keys[i].compare_exchange_strong(empty_ref,key);   // mint_compare_exchange_strong_32_relaxed(&m_entries[idx].key, 0, key);
//    if( (prevKey==0) || (prevKey==key))
//    {
//      m_vals[i].store(val);
//      return i;
//    }
//  }
//}

