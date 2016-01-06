/* ======================================================================================= 
   This file is released as part of GraviT - scalable, platform independent ray tracing
   tacc.github.io/GraviT

   Copyright 2013-2015 Texas Advanced Computing Center, The University of Texas at Austin  
   All rights reserved.
                                                                                           
   Licensed under the BSD 3-Clause License, (the "License"); you may not use this file     
   except in compliance with the License.                                                  
   A copy of the License is included with this software in the file LICENSE.               
   If your copy does not contain the License, you may obtain a copy of the License at:     
                                                                                           
       http://opensource.org/licenses/BSD-3-Clause                                         
                                                                                           
   Unless required by applicable law or agreed to in writing, software distributed under   
   the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY 
   KIND, either express or implied.                                                        
   See the License for the specific language governing permissions and limitations under   
   limitations under the License.

   GraviT is funded in part by the US National Science Foundation under awards ACI-1339863, 
   ACI-1339881 and ACI-1339840
   ======================================================================================= */
//
//  RayTracer.C
//

#include "MantaRayTracer.h"

#include <gvt/core/mpi/Wrapper.h>
#include <gvt/render/Types.h>
#include <gvt/render/adapter/manta/Wrapper.h>
#include <gvt/render/algorithm/Tracers.h>
#include <gvt/render/data/scene/Camera.h>
#include <gvt/render/data/scene/Image.h>
#include <gvt/render/Schedulers.h>

// Manta includes
#include <Interface/LightSet.h>
#include <Model/Lights/PointLight.h>
#include <Model/Materials/Phong.h>
#include <Model/Readers/PlyReader.h>
// end Manta includes

#include <boost/foreach.hpp>
#include <boost/timer/timer.hpp>

#ifdef PARALLEL
#include <mpi.h>
#endif

using namespace gvtapps::render;
using namespace gvt::core::mpi;
using namespace gvt::render::adapter::manta::data::domain;
using namespace gvt::render::data::domain;
using namespace gvt::render::data::scene;
using namespace gvt::render::schedule;

/// constructor
/**
 * \param cl configuration file loader for ray tracer initalization
 */
MantaRayTracer::MantaRayTracer(ConfigFileLoader& cl) : scene(&cl.scene)
{
    scene->camera.SetCamera(rays,1.0);
    
  gvt::render::RenderContext::CreateContext();
	cntxt = gvt::render::RenderContext::instance();   
	root = cntxt->getRootNode();
	gvt::core::Variant V;
	gvt::core::DBNodeH datanode = cntxt->createNodeFromType("Dataset","somedata",root.UUID());
	gvt::render::data::Dataset* ds = new gvt::render::data::Dataset();
    
    
    BOOST_FOREACH(AbstractDomain* dom, scene->domainSet) 
    {
        GeometryDomain* d = (GeometryDomain*)dom;
        d->setLights(scene->lightSet);
        ds->addDomain(new MantaDomain(d));
    }

    if (cl.accel_type != ConfigFileLoader::NoAccel)
    {
        std::cout << "creating acceleration structure... ";
        if (cl.accel_type == ConfigFileLoader::BVH)
        {
        	ds -> makeAccel();
        }
        std::cout << "...done" << std::endl;
    }
	V = ds;
	datanode["Dataset_Pointer"] = V;
	V = cl.scheduler_type;
	datanode["schedule"] = V;
	V = gvt::render::adapter::Manta;
	datanode["render_type"] = V;
	gvt::core::DBNodeH filmnode = cntxt->createNodeFromType("Film","somefilm",root.UUID());
    V = scene->camera.getFilmSizeWidth();
	filmnode["width"] = V;
	V = scene->camera.getFilmSizeHeight();
	filmnode["height"] = V;
}

/// render the image using the Embree ray tracer
/**
    \param imagename filename for the output image
*/
void MantaRayTracer::RenderImage(std::string imagename = "mpitrace") 
{
    
    boost::timer::auto_cpu_timer t("Total render time: %t\n");
 	int width = root["Film"]["width"].value().toInteger();   
 	int height = root["Film"]["height"].value().toInteger();   
    Image image(width,height, imagename);
    rays = scene->camera.MakeCameraRays();
	int stype = root["Dataset"]["schedule"].value().toInteger();
	if(stype == gvt::render::scheduler::Image) {
    	gvt::render::algorithm::Tracer<ImageScheduler>(rays, image)();  
	} else if (stype == gvt::render::scheduler::Domain) {
    	gvt::render::algorithm::Tracer<DomainScheduler>(rays, image)();  
	}

    gvt::render::algorithm::GVT_COMM mpi;
    if(mpi.root()) image.Write();
    

};

#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif



