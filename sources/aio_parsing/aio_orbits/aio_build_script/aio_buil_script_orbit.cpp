#include <aio_core/aio_build_script.h>
#include <aio_parsing/aio_orbits/aio_build_script/aio_build_script_orbit.h>
#include <aio_parsing/aio_orbits/aio_build_script/aio_particles/aio_build_script_domain_particle.h>
#include <aio_parsing/aio_orbits/aio_build_script/aio_particles/aio_build_script_config_particle.h>
#include <lib4aio_cpp_headers/utils/memory_utils/memory_utils.h>
#include <lib4aio_cpp_headers/utils/array_list_utils/array_list.h>

using namespace lib4aio;

aio_build_script_space *aio_build_script_orbit::new_space()
{
    return new_aio_build_script_materials();
}

array_list<aio_particle<aio_build_script_space>> *aio_build_script_orbit::create_particle_list()
{
    array_list<aio_particle<aio_build_script_space>> *list = new array_list<aio_particle<aio_build_script_space>>();
    aio_build_script_domain_particle *domain_particle = new aio_build_script_domain_particle();
    aio_build_script_config_particle *config_particle = new aio_build_script_config_particle();
    list->add(domain_particle);
    list->add(config_particle);
    return list;
}