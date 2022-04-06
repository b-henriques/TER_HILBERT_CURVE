import matplotlib.pyplot as plt
import numpy as np

x = np.array ()    # Données en abscisse 
y = np.array ()    # Données en ordonnée

plt.plot(x, y, 'x')     # Tracé de la courbe
plt.title('Mon titre')  # Ajout d'un titre
plt.xlabel('x')         # Nom de la grandeur en abscisse
plt.ylabel('y')         # Nom de la grandeur en ordonnée
plt.grid()              # Ajout d'une grille
plt.show()              # Affichage
