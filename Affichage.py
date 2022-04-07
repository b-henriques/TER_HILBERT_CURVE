import matplotlib.pyplot as plt
import numpy as np

cmake_minimum_required(VERSION 3.10)
project(pybind_vid)
add_subdirectory(pybind11)
pybind11_add_module(module_name main.cpp) // le nom du module et la bonne fonction

#include <pybind11/pydind11.h>

namespace = pybind11;

class UneClasse {
    float multiplier;

    public :
        float multiply(float input){
            return multiplier * input;
        };
    
    private : 

}

PYBIND11_MODULE(module_name, handle){
    handle.doc() = "This is the module docs";
    handle.def("nom_fonction_en_python", &nom_fonction);

    py::class_<SomeClass>(
                            handle, "PySomeClass"
                        )
        .def(py::init<)
}


// print(nom_fonction_en_python(3,5))

//print(PySomeCLass)






x = np.array ()    # Données en abscisse 
y = np.array ()    # Données en ordonnée

plt.plot(x, y, 'x')     # Tracé de la courbe
plt.title('Mon titre')  # Ajout d'un titre
plt.xlabel('x')         # Nom de la grandeur en abscisse
plt.ylabel('y')         # Nom de la grandeur en ordonnée
plt.grid()              # Ajout d'une grille
plt.show()              # Affichage

