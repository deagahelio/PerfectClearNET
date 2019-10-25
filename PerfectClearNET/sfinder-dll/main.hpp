#ifndef MAIN_H
#define MAIN_H

#define DLL extern "C" __attribute__((visibility("default")))

#include <sstream>
#include <vector>

#include "callback.hpp"
#include "core/field.hpp"
#include "finder/perfect.hpp"

core::Factory factory = core::Factory::create();
core::srs::MoveGenerator moveGenerator = core::srs::MoveGenerator(factory);
finder::PerfectFinder<core::srs::MoveGenerator> pcfinder = finder::PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

#endif