#ifndef Vxa_VMonotypeMapMakerFor_Instantiations
#define Vxa_VMonotypeMapMakerFor_Instantiations

/*************************************
 *****  Template Instantiations  *****
 *************************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vxa_VMonotypeMapMakerFor_Implementation)

#ifndef Vxa_VMonotypeMapMakerFor_Implementation
#define Vxa_VMonotypeMapMakerFor_Implementation extern
#endif

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<bool>;

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<short>;
Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<int>;

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<unsigned short>;
Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<unsigned int>;

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<float>;
Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<double>;

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<char const*, V::VString>;
Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<V::VString>;

#endif // defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vxa_VMonotypeMapMakerFor_Implementation)

#endif // #elif !defined(Vxa_VMonotypeMapMakerFor_Instantations) // #ifndef Vxa_VMonotypeMapMakerFor_Interface
