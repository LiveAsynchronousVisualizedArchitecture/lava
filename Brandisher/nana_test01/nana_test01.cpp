



#include <iostream>
#include <nana/gui/wvl.hpp>
#include <nana/gui/drawing.hpp>
#include <nana/paint/image_process_selector.hpp>
#include <nana/paint/pixel_buffer.hpp>
#include <nana/paint/graphics.hpp>
#include <../source/paint/detail/image_pixbuf.hpp>
#include <../source/paint/pixel_buffer.cpp>
#include <../source/paint/graphics.cpp>

#include "../../no_rt_util.h"

using namespace nana;
std::ostream& operator << (std::ostream& os, const point& p)
{
  return os << "point{" << p.x << "," << p.y << "}";
}
std::ostream& operator<< (std::ostream& os, const nana::size& z)
{
  return os << "size{" << z.width << "," << z.height << "}";
}
std::ostream& operator<< (std::ostream& os, const rectangle& r)
{
  return os << "rect{" << r.position() << "," << r.dimension() << "}";
}

class mem_pixbuf : public nana::paint::detail::basic_image_pixbuf
{
  bool open(const std::experimental::filesystem::path& file){ return false; }
  bool open(const void* data, std::size_t bytes){ return false; }
};

class tsform
  : public form
{
  nana::paint::image img_;
  paint::graphics gr_;
  mem_pixbuf mempb;
public:
  tsform()
  {
    
    //img_.image_ptr_ = mempb;

    //if (!img_.open(("../Examples/bground.6states.bmp")))
      //throw std::runtime_error("Imposible to open the image");    

    drawing dw(*this);
    dw.draw([this](paint::graphics& graph)
    {
      using namespace nana;
      using namespace nana::paint;

      //rectangle r { point{ 0,0 }, img_.size() };
      rectangle rw{ point{ 0,0 }, size() };
      //nana::size sz{size()};
      graph.rectangle(rectangle{ 5, 5, 50, 50 }, true, colors::red);
      graph.line(point(5, 5), point(55, 55), colors::blue);
      graph.line_begin(200, 100);
      graph.line_to(point(300, 300));
      graph.line_to(point(100, 200));
      graph.line_to(point(300, 200));
      graph.line_to(point(100, 300));
      graph.line_to(point(200, 100));
      graph.flush();
      graph.pixmap();

      //pixel_buffer& pxbuf = graph.impl_.get()->pxbuf;
      //pixel_buffer& pxbuf = graph.impl_->handle->pixbuf_ptr;
      pixel_argb_t* pxbuf = graph.impl_->handle->pixbuf_ptr;
      //auto* stor = pxbuf.storage_.get();
      //if(stor){
      if(pxbuf)
      {
        //auto rawpx = stor->raw_pixel_buffer;
        //auto     w = stor->pixel_size.width;
        //auto     h = stor->pixel_size.height;
        auto     w = graph.width();
        auto     h = graph.height();
        for(unsigned int y=0; y<h; ++y)
          for(unsigned int x=0; x<w; ++x)
          {
            pixel_argb_t p;
            p.element.red   = (u8)(y/(f32)h * 255.f);
            p.element.green = (u8)(x/(f32)w * 255.f);
            //p.element.red   = 0;
            //p.element.green = 0;
            p.element.blue  = 0;
            p.element.alpha_channel = 1;

            pxbuf[y*w + x] = p;
          }
      }



      //img_.paste(graph, 0, 0);
      //graph.p
      

      //img_.stretch(r, gr_, rw );
      //std::cout << "Pict: " << r << ", Windows: " << rw << "\n";
    });
    //      // get the graphycs !?
    //if (!API::window_graphics(*this, gr_))
    //      throw std::runtime_error("Imposible to get the graph");
    //      
    //      //Copy the image to the window
    //img_.paste( rectangle{ point{0,0}, img_.size() },
    //                        gr_,
    //                        {0,0});
    //img_.stretch( rectangle{ point{0,0}, img_.size() },
    //                        gr_,
    //                        size()  );
    dw.update();
    //Register events
    events().click  ( [this](){_m_click();} );  
    //events().resized( [this](){_m_size ();} );  
  }
private:
  //Switchs the algorithm between two algorithms in every click on the form.
  void _m_click()
  {
    static bool interop;
    nana::paint::image_process::selector sl;
    sl.stretch(interop ? "bilinear interoplation" : "proximal interoplation");
    interop = !interop;
    std::cout << (interop ? "Click: bilinear interoplation\n" 
      : "Click: proximal interoplation\n") ;
  }
  //  //When the window size is changed, it stretches the image to fit the window.
  //  void _m_size()
  //  {
  //      drawing dw(*this);
  //     
  //      dw.clear();  //Before stretch, it should clear the operations that are generated before.
  //      
  //img_.stretch(rectangle{ point{ 0,0 }, img_.size() },
  //      gr_,
  //      size());
  //dw.update();
  //std::cout << "resize\n";
  //  }
};
int main()
{
  tsform fm;
  fm.show();
  exec();
}

