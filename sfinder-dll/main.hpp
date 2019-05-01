#ifndef MAIN_H
#define MAIN_H

#include "Windows.h"
#define DLL extern "C" __declspec(dllexport)

#include <sstream>
#include <vector>

#include "core/field.hpp"
#include "finder/perfect.hpp"

core::Factory factory = core::Factory::create();
core::srs::MoveGenerator moveGenerator = core::srs::MoveGenerator(factory);
finder::PerfectFinder<core::srs::MoveGenerator> pcfinder = finder::PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

#endif