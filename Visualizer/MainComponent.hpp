

#ifndef __MAINCOMPONENT_HEADER_GUARD__
#define __MAINCOMPONENT_HEADER_GUARD__

#include "stdafx.h"
#include "DynLib.hpp"
#include "VizComponent.hpp"
#include "DataStore.hpp"
#include "SerializationUtil.hpp"
#include "DataVizProperties.hpp"
#include "Visualizer.h"

using namespace std::tr2::sys;

struct Viz;

class  MainComponent :   public Component,
                         public KeyListener
{
private:
  struct ValLis : public ValueListener
  {
    //int             idx       =   0;
    MainComponent*  parent   =   nullptr;
    DataType*     datatype   =   nullptr;
    Viz*               viz   =   nullptr;
    bool                on   =     false;
    int                idx   =        -1;
    int            propIdx   =        -1;
    string            name;

    ValLis(){}
    ValLis(string _name, MainComponent* _parent, DataType* _datatype, Viz* _viz, int _propIdx, bool _on=false) : 
      name(_name),
      parent(_parent),
      datatype(_datatype),
      viz(_viz),
      on(_on),
      idx(-1),
      propIdx(_propIdx)
    {} 

    void valueChanged(Value& value)
    {
      using namespace std;

      if(parent)
      {
        auto& vizComp  =  parent->vizComponent();
        if(!on)
        {
          auto& prop   =  parent->m_propCache[propIdx];
          prop.state   =  true;
          //parent->m_propCache[propIdx].state = true;
          idx          =  parent->vizComponent().nextIdx();

          MetaData md  =  Get(prop.id);
          md.dataType  =  parent->FindDataType(md.type);
          vizComp.command(nullptr, move(md) );
          //parent->requestDataByName(name);

          on    =  true;
        } 
        else
        {
          auto& prop   =  parent->m_propCache[propIdx];
          prop.state   =  false;
          on           =  false;
          //vizComp.destroyVisualization(name, viz);
          vizComp.destroyVisualization(prop.id);
          idx   =  -1;
        }
          
        vizComp.repaint();
      }
      //parent->m_propCache[propIdx].state = (bool)value.getValue();
      //
      //bool& cacheState  =  false;
      //auto& cache       =  parent->m_propCache;
      //if( propIdx .state ;
      //
      //cacheState  =  false;
      //cacheState  =  true;
    }
  };
  struct ExtType
  {
    string extension;
    string type;
  };
  struct NamePath
  {
    string name;
    string filePath;
  };
  struct Prop
  {
    i64           id;
    string      name;
    string      type;
    ui64        hash;
    Viz*         viz;
    DataType*     dt;
    int          idx;
    bool      update;
    bool       state;
    bool   openState;
    int     stateIdx;
  };

  using  Strings      =  vec<string>;
  using  Paths        =  vec<path>;
  using  ExtTypes     =  vec<ExtType>;
  using  TimedCall_t  =  TimedCall<void(void)>;
  using  VizIdxs      =  hashmap<string, int>;
  using  VizC         =  VizComponent;
  using  LayManager   =  StretchableLayoutManager;
  using  LayResizer   =  StretchableLayoutResizerBar;
  using  uLayResizer  =  unq< LayResizer >;
  using  CS           =  ColorSamplesBase<>;
  using  Types        =  vec<string>;
  using  VizByTypes   =  map<string, Viz*>;
  using  DataByTypes  =  map<string, DataType*>;
  using  DataNames    =  multimap<string, NamePath>;
  using  NTHs         =  DataStore::NTHs;          // NameTypeHashs
  using  PropCache    =  vec<Prop>;
  using  DataTypes    =  vec<DataType*>;
  using  CPaths       =  const Paths&;
  using  CExtTypes    =  const ExtTypes&;
  using  CStrings     =  const Strings&;
  using  Cstring      =  const string&;
  using  CMemBlock    =  const MemoryBlock&; 
  using  CDynLib      =  const DynLib&;

  CS                   m_cs;
  vec<DynLib>          m_vizLibs;                 // visualization libraries
  vec<DynLib>          m_dtLibs;                  // data type libraries
  DataByTypes          m_dByTypes;
  TimedCall_t          m_libReloader; 
  TimedCall_t          m_serverQuery;
  TimedCall_t          m_memQuery;

  DataVizProperties    m_pp;
  TextEditor           m_txt;
  PropCache            m_propCache;
  bool                 m_updateProperties;

  VizC                 m_vizComp;
  VizIdxs              m_vizIdxs;
  ValLis               m_vizLis;

  LayManager           m_vizLayout;
  uLayResizer          m_propResizer;      // doesn't throw leaked exception
  LayManager           m_infoLayout;
  uLayResizer          m_infoResizer;

  DataServer           m_dataServer;       // change to m_server?
  ClientConnection     m_client;
  ui64                 m_namesHash    =  0;

  ABool                m_refreshReady = true;

  DataType*                GetDatas(DynLib&       lib)
  {
    if(!lib.empty()) {
      void* datasPtr = lib.getFunction("GetDataTypes");
      return ((GetDataTypes_t)datasPtr)();
    }
    return nullptr;
  }
  DataTypes         GetAllDataTypes()
  {
    DataTypes ret;
    for(auto& l : m_dtLibs)
    {
      auto dt = GetDatas(l);
      for(;dt;++dt) ret.push_back(dt);
    } 
    return ret;
  }
  DataType*            FindDataType(Cstring      type)
  {
    for(auto& l : m_dtLibs)
    {
      auto dt = GetDatas(l);
      for(;dt && dt->type; ++dt) 
        if(type == dt->type)
          return dt;
    } 
    return nullptr;
  }
  bool              IsValidDataType(DataType*      dt)
  {
    return dt!=nullptr && dt->type!=nullptr; // && dt->name!=nullptr;  //dt->extension!=nullptr;
  }
  ExtTypes       GetValidExtensions(CExtTypes  dtExts, CStrings   types) const
  {
    using namespace cpplinq;

    return from(dtExts)
      >> where([&types](ExtType const& te){return 
            from(types) 
              >> cpplinq::contains(te.type) // [](ExtType const& a, ExtType const& b)
                  //{return a.extension==b.extension;} ) 
            ;} )
      >> to_vector();
      ;
  }
  vec<path>     GetDataTypeLibPaths()
  {
    return GetExtensionPaths("_datatype.dll");  // dtp = data type paths
    //for(auto const& p : dtp) {
    //  m_dtpLibs.emplace_back(p, 0.0);
    //}
  }
  void                     FillLibs()
  {
    //auto vp   =  GetExtensionPaths("_viz.dll");       // vp = viz paths
    //for(auto const& p : vp) {
    //  m_vizLibs.emplace_back(p, 0.0);
    //}

    //auto dtp  =  GetExtensionPaths("_datatype.dll");  // dtp = data type paths
    //for(auto const& p : dtp) {
    //  m_dtpLibs.emplace_back(p, 0.0);
    //}
  }
  void               RemoveLiveLibs()
  {
    for(auto& l : m_vizLibs)  l.removeLive();
    for(auto& l : m_dtLibs)   l.removeLive();
  }
  ExtTypes    GetDataTypeExtensions()                                    const
  {
    ExtTypes ret;
    for(auto& lib : m_dtLibs)
    {
      void*  fp  =  lib.getFunction("GetDataTypes");
      auto  dts  =  ((GetDataTypes_t)fp)();
      for(; dts            != nullptr && 
            //dts->extension != nullptr &&
            dts->type      != nullptr; ++dts)
        ret.push_back( {dts->type,dts->type} );  
    }
    return ret;
  }
  Paths           KeepFromExtension(Paths       paths, CExtTypes   exts) const
  {
    vec<path> ret;
    for(auto& p : paths)
    {
      for(auto& e : exts)
        if( hasExtension(p.string(), e.extension) ) {
          ret.emplace_back(move(p));
          break;
        }
    }
    return ret;
  }
  auto           DataNamesFromFiles(CPaths  dataFiles, CExtTypes dtExts) const -> DataNames
  {
    using namespace cpplinq;

    //auto ret = from(dataFiles) 
    //  >> select([](path const& p){ return p.filename(); })
    //  >> where([&dtExts](string const& s){return 
    //       from(dtExts) 
    //         >> any([&s](ExtType const& e){return hasExtension(s,e.extension);}) ;}) 
    //  ;
    //>> select([](path const& p){return p.filename();})
    //auto matchExt = from(dtExts)
    //  >> first([&f](ExtType const& e)
    //       {return hasExtension(f.filename(),e.extension);} )
    //;

    DataNames ret;
    for(auto& f : dataFiles)
    {
      for(auto& ne : dtExts)
      {
        if( hasExtension(f.string(), ne.extension ) )
        { 
          string s = f.filename();
          s.resize( s.rfind(ne.extension,s.size()) );
          ret.insert( {ne.type, {s, f.string()}} );
          //ret.emplace_back( move(s) );
          break;
        }
      }
    }
    return ret;
  }
  Prop                  PropFromKey(i64           key)
  {
    Prop  p;
    auto  dataTypes  =  GetDataTypeByTypes();
    str        type  =  typedb.getPtr(key);
    p.dt             =  dataTypes[type];
    p.viz            =  nullptr;
    if(p.dt)
    {
      p.id         =  key;
      p.name       =  namedb.getPtr(key);
      p.type       =  move(type);
      p.hash       =  bindb.getTime(key);
      p.update     =  true;
      p.stateIdx   =  -1;
      p.state      =  false;     // need to connect this to the actual state
    }
    return p;
  }
  void               SendVizCommand(i64           key)
  {
    MetaData md  =  Get(key);
    md.dataType  =  this->FindDataType(md.type);
    m_vizComp.command(nullptr, move(md) );
  }
  bool                      Refresh()
  {
    // todo: redo this and be able to query time once?
    ReadyGuard ready(&m_refreshReady);
    if( !ready() ) return false;

    bool   newBin = false;               // Is there a new binary (and not just an update to an old one)

    void*     rec = bindb.first();       // todo: redo this to lock whitedb
    for(;rec; rec = bindb.next(rec) )
    {
      auto     key     =   bindb.getKey(rec);
      if(key<0) continue;

      char*   typePtr  =   typedb.getPtr(key);
      string  type     =   typePtr?  typePtr  :  "";
      if(type=="IndexedVerts")
      {
        i64    time    =   bindb.getTime(key);
        char*  prev    =   timedb.getPtr(key);
        i64    pt      =   prev?  *((i64*)prev)  :  0;
        if(!prev)
        {
          PutTime(key, time);
          m_propCache.push_back( PropFromKey(key) );
          newBin = true;
        }
        else if(time != pt)
        {
          PutTime(key, time);
          
          //bool send=true;
          for(auto& p : m_propCache)
            if(p.id==key && p.state) { SendVizCommand(key); break; }   //  { send=false; break; }

          //if(send) SendVizCommand(key);
        }
      }
    }
    return newBin;
  }


public:
  MainComponent()
  {
    auto ths = this;
    
    FillLibs();
    ths->ReloadLibs();
    m_libReloader.func  =  [ths](){ ths->ReloadLibs(); };
    m_libReloader.startTimer(100);

    m_infoResizer.reset(new LayResizer(&m_infoLayout,1,false));
    m_infoLayout.setItemLayout(0,   -0.20,  -0.975,  -0.95 );
    m_infoLayout.setItemLayout(1,       8,       8,      8 );
    m_infoLayout.setItemLayout(2,  -0.025,   -0.80,  -0.05 );

    m_propResizer.reset(new LayResizer(&m_vizLayout,1,true));
    m_vizLayout.setItemLayout(0, -0.25,  -0.90,  -0.65 );
    m_vizLayout.setItemLayout(1,     8,      8,      8 );
    m_vizLayout.setItemLayout(2, -0.10,  -0.75,  -0.35 );

    m_txt.setMultiLine(true, false);
    m_txt.setReturnKeyStartsNewLine(true);

    addAndMakeVisible(m_vizComp,  0);
    addAndMakeVisible(m_pp,       1);
    addAndMakeVisible(m_txt,      2);
    addAndMakeVisible(m_propResizer.get());
    addAndMakeVisible(m_infoResizer.get());

    addKeyListener(this);

    setSize(768, 512);
    resized();
      
    m_memQuery.func = [ths]()
    {
      static ABool readyFlag   =   true;
      static Ai32  cnt         =   0;

      //ScopeCount   sc(&cnt);

      ReadyGuard   ready(&readyFlag);
      if(!ready()) return;            // don't overlap the timed queries
 
      bool ok = ths->Refresh(); 
      if(ok) 
        ths->RefreshProperties();
    };
    m_memQuery.startTimer(1000);
  }
  ~MainComponent()
  {
    RemoveLiveLibs();
    //for(auto& lib : m_libs) 
    //  lib.removeLive();    
  }
  void                        paint(Graphics&      g)  override
  {          
  }
  void                      resized()                  override
  {
    m_propResizer->setBounds( this->getBounds() );
    m_infoResizer->setBounds( this->getBounds() );
    m_pp.setBounds(           this->getBounds() );
    m_vizComp.setBounds(      this->getBounds() );
    m_txt.setBounds(          this->getBounds() );

    Component* infoComps[] = { &m_vizComp, m_infoResizer.get(), &m_txt };
    m_infoLayout.layOutComponents(infoComps, 3, 0,0,getWidth(),getHeight(),true,false);

    int vizPos          =  m_infoLayout.getItemCurrentPosition(0);
    int vizHeight       =  m_infoLayout.getItemCurrentAbsoluteSize(0);
    Component* comps[]  =  { &m_vizComp, m_propResizer.get(), &m_pp };
    m_vizLayout.layOutComponents(comps, 3, 0,vizPos,getWidth(),vizHeight,false,false);
      
    m_propResizer->resized();

    repaint();
  }
  bool                   keyPressed(const KeyPress &key, Component *originatingComponent) override
  {
    if( key.getKeyCode() ==  KeyPress::spaceKey )
    {
      m_vizComp.resetCam();
      m_vizComp.repaint();
    }
    if( key.getKeyCode() ==  KeyPress::pageUpKey )
    {
      m_vizComp.incPtSz();
    }
    if( key.getKeyCode() ==  KeyPress::pageDownKey )
    {
      m_vizComp.decPtSz();
    }

    //addText( toString(key.getKeyCode()) );

    return false;
  }
  string            VizPropertyName(CDynLib      lib)  const
  {
    path   pth     =  lib.origPath();
    pth.replace_extension("");
    string pthStr  =  pth.filename();
      
    return pthStr;
  }
  void          loadAllDataTypeLibs()
  {
    for(auto& lib : m_dtLibs) lib.load();
  }
  void               loadAllVizLibs()
  {
    for(auto& lib : m_vizLibs) lib.load();
  }
  void            RefreshProperties()
  {
    using namespace std;
    
    XmlElement* opens = m_pp.getOpennessState();
    m_pp.clear();
    TO(m_propCache.size(),i)
    {
      auto& p     =  m_propCache[i];
      p.stateIdx  =  (int)i;
      m_pp.dvAddProperty(p.openState, p.name, p.state, 
                         "Viz Name", "", 
                         new ValLis(p.name,this,p.dt,p.viz, (int)i, p.state ) );
      p.update    =  false;
    }
    m_pp.restoreOpennessState(*opens);
    delete opens;
  }
  void                   ReloadLibs()
  {
    using namespace cpplinq;

    static Ai64 callCnt = 0;

    // hard code this first then make it general
    void* cs  =  &m_cs;  
    auto dataFilesPath  =  JuceExecDir()/path("data");
    auto allFiles       =  AllFiles(dataFilesPath);

    // get the datatype library files
    auto dtLibPaths     =  GetDataTypeLibPaths();
      
    // construct the new datatype libraries
    for(auto& lp : dtLibPaths)  // break the library loading up from the property adding
    {
      bool found = false;
      if(found) break;
      for(auto& lib : m_dtLibs) 
        if(lib.origPath()==lp){ found=true; break; }
        
      if(!found) m_dtLibs.emplace_back( lp );
    }
    loadAllDataTypeLibs();  // break these out into a separate function?
    loadAllVizLibs();

    //// get vec of file extensions
    //auto      dtExts       =  GetDataTypeExtensions();
    //auto      types        =  GetVizTypes();
    //
    //// figure out valid extensions
    //auto extensions        =  GetValidExtensions(dtExts, types); 
    //
    //// and cull out all the file extensions that won't work
    //vec<path>   dataFiles  =  KeepFromExtension(move(allFiles), extensions);
    //DataNames   dataNames  =  DataNamesFromFiles(dataFiles,     extensions);
    //auto        vizTypes   =  GetVizsByTypes();
    //auto        dataTypes  =  GetDataTypeByTypes();
    //
    //if(callCnt < 1)
    //  for(auto& kv : vizTypes)
    //  {
    //    auto&  typeStr   =  kv.first;
    //    auto   viz       =  kv.second;
    //    auto   dt        =  dataTypes[typeStr];
    //    auto&  vizName   =  viz->name;
    //    auto   rng       =  dataNames.equal_range(typeStr);
    //    auto   st        =  rng.first;
    //    auto   end       =  rng.second;
    //
    //    for(auto iter=st; iter!=end; ++iter)
    //    {
    //      auto&  np      =  iter->second;  // name and path
    //      int shapeIdx   =  0;
    //      auto fullName  =  toString(np.name,"  :  ",typeStr);
    //      m_pp.dvAddProperty(fullName, false, vizName, "", new ValLis(np.filePath,this,dt,viz) );
    //    }
    //  }

    //for(auto& lib : m_vizLibs) ReloadLib(lib,cs);
    //for(auto& lib : m_dtpLibs) ReloadLib(lib,cs);

    callCnt.fetch_add(1);
  }
  auto                 vizComponent() -> VizC&
  { 
    return m_vizComp; 
  }
  void                      addText(Cstring        s)
  {
    m_txt.setText( m_txt.getText() + s );
  }
  void            requestDataByName(Cstring     name)
  {
    //auto nameBytes = SerializeToBytes<ArOut,string>(name);
    //m_client.sendMessage(DataStore::DATA_OBJ, nameBytes);

    //m_client.sendMessage(DataStore::IS_VISIBLE, nameBytes);
  }
  auto           GetDataTypeByTypes() -> DataByTypes
  {
    using namespace cpplinq;
      
    DataByTypes ret;
    for(auto& lib : m_dtLibs)
    {
      DataType* dt = GetDatas(lib);
      for(; IsValidDataType(dt); ++dt)
        ret[dt->type] = dt;
    }
    return ret;
  }
  bool         CompareAndUpdateHash(CMemBlock    msg)
  {
    //auto bytes  =  MemoryBlockToVec(msg);
    //ui64 hsh    =  std::hash<vec<ui8>>()(bytes);
    //ui64 hsh = 0;
    //TO(bytes.size(),i) hsh ^= std::hash<ui8>()(bytes[i]);

    auto hsh = HashBytes(msg.getData(), msg.getSize() );

    if(hsh == m_namesHash) return false;
    else {
      m_namesHash = hsh;
      return true;
    }
  }
  bool                         isOn(Cstring     name)
  {
    bool isOn = false;
    for(auto& p : m_propCache)
      if(p.name == name){ 
        isOn = p.state; 
        break; 
      }
    return isOn;
  }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};


#endif































//if(!prev) timedb.put(key, )
//if(!prev || time != pt )

//ui8*   t     =  (ui8*)&time;
//timedb.put(key, { t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7] } );

//auto name  =  string(namedb.getPtr(key));
//auto nth   =  NameTypeHash(key, move(name), move(type), "", "");
//nth.hsh    =  time;
//nths.push_back( move(nth) );

//auto dataTypes = GetDataTypeByTypes();
//for(auto& nth : nths)
//{
//  bool update = false;
//  TO(m_propCache.size(), i)
//  {
//    auto& p = m_propCache[i];
//    if( nth.name == p.name || nth.hsh == p.hash ) // update
//    {
//      p.id      =  nth.id;
//      p.name    =  nth.name;
//      p.type    =  nth.type;
//      p.hash    =  nth.hsh;
//      p.dt      =  dataTypes[nth.type];
//      //p.viz     =  vizTypes[nth.type];
//      p.update  =  true;
//      
//      update    =  true;
//      break;
//    }
//  }
//  if(!update) // new nth
//  {
//    Prop p;
//    p.dt      =  dataTypes[nth.type];
//    p.viz     =  nullptr;
//    if(p.dt) {
//      p.id        =  nth.id;
//      p.name      =  nth.name;
//      p.type      =  nth.type;
//      p.hash      =  nth.hsh;
//      p.update    =  true;
//      p.stateIdx  =  -1;
//      p.state     =  false;  // need to connect this to the actual state
//      m_propCache.push_back(p);
//    }
//  }
//}
//erase_if(m_propCache,[](Prop const& p){return !p.update;});

//NTHs     nths;
//void*     rec = bindb.first();
//for(;rec; rec = bindb.next(rec) )
//{
//  auto   key       =  bindb.getKey(rec);
//  if(key<0) continue;
//
//  auto   type      =  string(typedb.getPtr(key));
//  if(type=="IndexedVerts")
//  {
//    i64   time     =     bindb.getTime(key);
//    char* prev     =     timedb.getPtr(key);
//    i64   pt       =     prev? *((i64*)prev) : 0;       // pt is previous time
//    //if(!prev) timedb.put(key, )
//    //if(!prev || time != pt )
//    if(pt==0 || time != pt )
//    {
//      ui8*   t     =  (ui8*)&time;
//      timedb.put(key, { t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7] } );
//      auto   name  =  string(namedb.getPtr(key));
//      auto   nth   =  NameTypeHash(key, move(name), move(type), "", "");
//      nth.hsh      =  time;
//      nths.push_back( move(nth) );
//    }
//  }
//}
//ths->RefreshProperties(nths);

//static void   ClientCallback(MainComponent* ths, MemoryBlock const& msg)
//{        
//  void*  ptr   =  ((ui8*)msg.getData()) + 1;
//  auto   len   =  msg.getSize() - 1;
//  if(msg.getSize() <= 0) return;
//
//  switch(msg[0])
//  {
//    case DataStore::AVAILABLE_TYPES:
//    {
//      auto nths = DataStore::unserializeTypeStrs(ptr, len);  // names types hashs
//
//      if( ths->CompareAndUpdateHash(msg) )
//        ths->RefreshProperties(nths);
//        
//      break;
//    }
//    case DataStore::DATA_OBJ:
//    case DataStore::UPDATE_OBJ:
//    {
//      auto md      =  DataStore::unserialize<MetaData>(ptr, len);
//      md.dataType  =  ths->FindDataType(md.type);
//      
//      if(ths->isOn(md.name)) 
//        ths->vizComponent().command(nullptr,move(md));
//      
//      break;
//    }
//    case DataStore::IS_VISIBLE:
//    {
//      string name =  DataStore::unserialize<string>(ptr, len);
//      if(ths->isOn(name))
//      {
//        //uint8_t trueVal = 0x01;
//        //DataStore::DataBytes trueVal(1, 0x01);
//        auto nameBytes = SerializeToBytes<ArOut,string>(name);
//        ths->m_client.sendMessage(DataStore::IS_VISIBLE, move(nameBytes));
//      }
//    }
//    //{
//    //  auto md  =  DataStore::unserialize<MetaData>(ptr, len);
//    //  ths->vizComponent().command(nullptr,md);
//    //  break;
//    //}
//    default:
//      ;
//  }
//}

//m_client.setCallback([ths](MemoryBlock msg)
//{ MainComponent::ClientCallback(ths, msg); });
//auto con = m_client.connect();
//ths->m_client.sendMessage(DataStore::AVAILABLE_TYPES, {} );
      
//m_serverQuery.func = [ths]()
//{
//  ths->m_client.sendMessage(DataStore::AVAILABLE_TYPES, {} );
//};
//m_serverQuery.startTimer(1000);

//if(!p.state) p.stateIdx  = -1;
//else         p.stateIdx  =  0;

//auto vizTypes  = GetVizsByTypes();
//p.viz     =  vizTypes[nth.type];
//if(p.dt && p.viz) {

//Viz*          GetVizs(DynLib& lib)
//{
//  if(!lib.empty()) {
//    void* vizsPtr = lib.getFunction("GetVisualizations");
//    return ((GetVisualizations_t)vizsPtr)();
//  }
//  return nullptr;
//}
//bool          IsValidViz(Viz* v)
//{
//  return v!=nullptr && v->type!=nullptr && v->name!=nullptr;
//}
 
//Strings       GetVizTypes()
//{
//  using namespace cpplinq;
//
//  //return from(m_vizLibs) 
//  //  >> generate([](DynLib const& l){ })
//
//  Strings ret;
//  for(auto& lib : m_vizLibs)
//  {
//    Viz* v = GetVizs(lib);
//    for(; IsValidViz(v); ++v)
//      ret.emplace_back(v->type);
//  }
//  return ret;
//}

//ui64          VizLength(Viz* v)                   const
//{
//  int   vizLen   =  0;
//  Viz*  vizIter  =  v;
//  for(;vizIter!=nullptr && vizIter->type!=nullptr; ++vizIter, ++vizLen){} // relies on shortcircuiting
//    
//  return vizLen;
//}

//VizByTypes    GetVizsByTypes()
//{
//  using namespace cpplinq;
//    
//  VizByTypes ret;
//  for(auto& lib : m_vizLibs)
//  {
//    Viz* v = GetVizs(lib);
//    for(; IsValidViz(v); ++v)
//      ret[v->type] = v;
//  }
//  return ret;
//}

//
//this->addText( m_vizComp.status() + "\n" );

//m_pp.saveState();
//for(auto& p : m_propCache)
//m_pp.dvAddProperty(p.name, false, p.viz->name, "", new ValLis(p.name,this,p.dt,p.viz) );
//m_pp.restoreState();

//md.dataType  =  ths->GetDataTypeByTypes()[md.type];
//auto viz     =  ths->GetVizsByTypes()[md.type];
//ths->vizComponent().command(viz,md);

//using Ar = cereal::PortableBinaryInputArchive;
// get idx from vizComp?
//
//MemoryBlock fileData;
//fileData.reset();
//bool  ok    =  File(filePath.string()).loadFileAsData(fileData);
//
//void* data  =  nullptr;
//datatype->data_create(&data, fileData.getData(), fileData.getSize());
//  idx       =  vizComp.addVisualization(viz, (void*)data);
//datatype->data_destroy(data);
//
//vizComp.destroyVisualization(idx);

//vec<path>     GetExtensionPaths(string const& extension) const
//{
//  vec<path> ret;
//
//  directory_iterator libDir( JuceExecDir() );
//  directory_iterator endDir;
//  for(; libDir!=endDir; libDir++ )
//  {
//    auto& pstr = libDir->path().string();
//    if( pstr.rfind(extension, pstr.size()) < pstr.size() )
//      ret.emplace_back( libDir->path() );
//  }
//  return ret;
//}

