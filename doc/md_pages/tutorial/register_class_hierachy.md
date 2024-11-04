Register Class Hierarchy {#rttr_type_class_hierachy_page}
========================

Within the current C++ standard it is not possible to extract a class hierarchy automatically from a certain type. 
Therefore the programmer has to put a certain macro inside every class in order to provide this information. 
Additionally this macro will be needed to retrieve the information about the most derived [type](@ref rttr::type) of a current instance.

Macros you have to insert in the class declaration are named: @ref RTTR_DECLARE_ROOT() or @ref RTTR_DECLARE_ANCESTORS()

Suppose we have a base struct called `Base`:
~~~~{.cpp}
struct Base
{
    RTTR_DECLARE_ROOT()
};
~~~~
Place the macro \ref RTTR_DECLARE_ROOT() somewhere in the class, it doesn't matter if its under the public, protected or private class accessor section.

Into the derived class you put \ref RTTR_DECLARE_ANCESTORS() macro, with the name of the parent class as argument.
Which is in this case `Base`.
~~~~{.cpp}
struct Derived : Base
{
    RTTR_DECLARE_ANCESTORS(Base)
};
~~~~

When you use multiple inheritance you simply separate every class with a comma.
~~~~{.cpp}
struct MultipleDerived : Base, Other
{
    RTTR_DECLARE_ANCESTORS(Base, Other)
};
~~~~
Remark that the order in which you declare here the multiple inheritance, has an impact later when retrieving properties of a class.
So it is best practice to use the same order like in your class.
RTTR supports to register even virtual base classes. 
@remark The only limitation you have is: It is **not** possible to register a class twice in the same class hierarchy.

When no class hierarchies are used at all, it is **not necessary** to use the macro. However it is best practice to place it inside every class.
This macro will also enable the possible usage of an own cast operator called: `rttr_cast`.
How this will be done, is discussed in the [next](@ref rttr_type_rttr_cast_page "Using rttr_cast") chapter.

Summary
-------
- to retrieve meta information of derived and base classes it is necessary to place macros @ref RTTR_DECLARE_ROOT()
  or @ref RTTR_DECLARE_ANCESTORS() inside every class declaration
- the macro is **not** needed when working with classes which will be not inherited, e.g. POD classes

<hr>

<div class="btn btn-default doxy-button">[previous](@ref rttr_type_info_page "Query information from rttr::type")</div><div class="btn btn-default doxy-button">[next](@ref rttr_type_rttr_cast_page "rttr_cast vs. dynamic_cast")</div>
