import pyHilbertCurve
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
from enum import Enum

def printHilbertCurve(order=1, n_threads=1):
    """ shows HilbertCurve of specified order 
    """

    # gen points
    points = pyHilbertCurve.VectorPoint2D()
    val = (10 / (2**(order+1)))
    for x in range(0, 2**order):
        for y in range(0, 2**order):
            points.append(pyHilbertCurve.Point2D((2*x+1)*val, (2*y+1)*val))

    # create curve, order points
    hc = pyHilbertCurve.HilbertCurve2D_Fixed(points, order, 10, 10, n_threads)

    # retriev x,y coords
    x_coords = []
    y_coords = []
    for p in points:
        x_coords.append(p.getX())
        y_coords.append(p.getY())

    # print curve
    plt.xlim([0, 10])
    plt.ylim([0, 10])
    plt.plot(x_coords, y_coords, label=f'HilbertCurve Order {order}')
    plt.show()


class Distribution(Enum):
    UNIFORM = 1
    CENTERED = 2
    EXTREMES = 3


def show_repartition(order=1, nb_of_points=4, distribution=Distribution.UNIFORM, n_threads=1, annotations = False):
    pt_gen_builder = pyHilbertCurve.PointsGeneratorBuilder()

    gen = 0
    if(distribution == Distribution.UNIFORM):
        gen = pt_gen_builder.mk_uniform(pyHilbertCurve.Point2D(10, 10))
    else:
        if(distribution == Distribution.CENTERED):
            gen = pt_gen_builder.mk_gamma(
                pyHilbertCurve.Point2D(10, 10), [5.0, 1.0], [5.0, 1.0])
        else:
            x_params = ([0.0, 2.0, 8.0, 10.0], [5.0, 0.5, 5.0])
            y_params = ([0.0, 2.0, 8.0, 10.0], [5.0, 0.5, 5.0])
            gen = pt_gen_builder.mk_piecewise_constant(x_params, y_params)

    points = gen.generatePoints(nb_of_points, n_threads)
    points2 = pyHilbertCurve.VectorPoint2D(points)

    # create curve, order points
    hc_fixed = pyHilbertCurve.HilbertCurve2D_Fixed(
        points, order, 10, 10, n_threads)
    hc_adp = pyHilbertCurve.HilbertCurve2D_Adaptive(
        points2, order, 10, 10, n_threads)

    fixed = []
    adp = []
    for y in range(2**order-1, -1, -1):
        f = []
        a = []
        for x in range(0, 2**order):

            hi = hc_fixed.coords_to_hilbertindex(x, y)

            (start, end) = hc_fixed.get_points_from_hilbertindex(hi)
            f.append(end - start + 1)

            (start, end) = hc_adp.get_points_from_hilbertindex(hi)
            a.append(end - start + 1)

        fixed.append(f)
        adp.append(a)


    fig, (ax1, ax2) = plt.subplots(1, 2)
    fig.suptitle("Distribution Comparison")
    ax1.set_title("Fixed")
    ax2.set_title("Adaptive")
    
    plt.subplots_adjust(wspace = 0.05)

    sns.heatmap(fixed, ax=ax1, linewidth=0.1, linecolor='blue', annot=annotations, cmap=plt.cm.Blues)
    sns.heatmap(adp, ax=ax2, linewidth=0.1, linecolor='blue', annot=annotations, cmap=plt.cm.Blues)
    plt.show()

