

#ifndef __DATAVIZPROPERTIES_HEADER_GUARD__
#define __DATAVIZPROPERTIES_HEADER_GUARD__

// need proper includes

class DataVizProperties : public PropertyPanel
{
private:
  using AryProp    =  Array<PropertyComponent*>;
  using Sections   =  hashmap<string, AryProp>;
  using SecIdxs    =  hashmap<string, int>;
  using SaveState  =  hashmap<string, bool>;
  using Listeners  =  vec< unq<ValueListener> >;
  using Values     =  list< Value >;              // doesn't have to move elements because it doesn't resize

  Sections    m_sections;
  SecIdxs     m_secIdxs;
  Listeners   m_listeners;
  Values      m_vals;
  SaveState   m_state;

public:
  DataVizProperties() : PropertyPanel() {}
  
  void clear()  // need to destroy visualizations here?
  {
    //m_propData.clear();
    m_sections.clear();
    m_secIdxs.clear();
    m_vals.clear();
    //m_listeners.clear();
    //this->clear();
    PropertyPanel::clear();
  }
  bool isTurnedOn(string const& name)
  {
    
  }
  void dvAddProperty(
    bool open                =  true,
    string const&  section   =  "",
    bool           firstVal  =  false, 
    string const&  label     =  "", 
    string const&  name      =  "", 
    ValueListener* lis       =  nullptr)
  {
    if(m_sections.count(section)>0)
      this->removeSection(m_secIdxs[section]);

    m_vals.emplace_back( var(firstVal) );
    m_vals.back().addListener(lis);
    auto bpc  =  new BooleanPropertyComponent(m_vals.back(), label, name);
    auto ary  =  AryProp();
    ary.add(bpc);

    this->addSection(section, ary, open, -1);
    m_secIdxs[section] = this->getSectionNames().size() - 1;

    //bpc->setState( !firstVal );
    //bpc->setState(  firstVal );
  }

};


#endif








  //void getState(int secIdx, int idx)
  //{
  //  auto n = this->getSectionNames()[secIdx];
  //  this->get
  //}

  //void saveState()
  //{
  //  //SaveState ret;
  //  //for(auto& kv : m_secIdxs)
  //  //for(auto& v : m_vals)
  //  //auto& names = this->getSectionNames();
  //  //for(auto& n : names)
  //  //{
  //  //  auto idx = m_secIdxs[n.toStdString()];
  //  //  //auto idx  =  kv.second;
  //  //  //v.
  //  //  ret[idx]  =  v.getValue()
  //  //}
  //
  //  m_state.clear();
  //  auto& nvs = this->getProperties();
  //  //TO(nvs.size(),i)
  //  TO(m_vals.size(),i)
  //  {
  //    auto& nv       =  nvs.getValueAt(i);
  //    auto  name     =  nvs.getName(i).toString().toStdString();
  //    m_state[name]  =  (bool)nv;
  //  }
  //}
  //void restoreState()
  //{
  //  //m_state.clear();
  //  auto& nvs = this->getProperties();
  //  TO(nvs.size(),i)
  //  {
  //    //auto& nv    =  nvs.getValueAt(i);
  //    auto nvptr  =  nvs.getVarPointerAt(i);
  //    auto name   =  nvs.getName(i).toString().toStdString();
  //    *nvptr      =  m_state[name];
  //    //nvs.set(     =  m_state[name];
  //  }
  //}
