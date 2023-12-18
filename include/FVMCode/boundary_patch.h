#ifndef BOUNDARY_PATCH_H
#define BOUNDARY_PATCH_H

#include <string>
#include <unordered_map>

#include "exceptions.h"

namespace FVMCode
{

enum BoundaryType
{
    wall,
    empty
};

const std::unordered_map<std::string, BoundaryType> string_to_type ({
    { "wall",  wall},
    {"empty", empty}
});

struct BoundaryPatch
{
    const std::string  name;
    const BoundaryType type;
    const unsigned int n_faces;
    const unsigned int start_face;

    BoundaryPatch (const std::string &name, BoundaryType type,
                   const unsigned int n_faces, const unsigned int start_face)
        : name (name)
        , type (type)
        , n_faces (n_faces)
        , start_face (start_face)
    {
    }

    BoundaryPatch (const std::string &name, const std::string &type,
                   const unsigned int n_faces, const unsigned int start_face)
        : BoundaryPatch (name, type_from_string (type), n_faces, start_face)
    {
    }

    static BoundaryType type_from_string (const std::string &type_string)
    {
        Assert (string_to_type.find (type_string) != string_to_type.end (),
                "Invalid type detected");
        return string_to_type.at (type_string);
    }
};

class BoundaryFieldEntry
{
  public:
    std::string type;
    double      value;

    BoundaryFieldEntry (const std::string &type, const double value)
        : type (type)
        , value (value)
    {
    }
};

} // namespace FVMCode

#endif