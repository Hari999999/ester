#ifndef MATPLOTLIB_H
#define MATPLOTLIB_H

#ifndef WITH_CMAKE
#include "ester-config.h"
#endif

#if ENABLE_PLT
#include <Python.h>
#else
#define PyObject void
#endif

#ifdef USE_VTK
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>
#endif

#include "utils.h"
#include "matrix.h"

#include <vector>

class plt {
    private:
        static std::map<std::string, PyObject *> py;
        static std::vector<std::string> functions;
        static void call(const std::string& name,
                PyObject *args = NULL,
                PyObject *kwargs = NULL);
        static void block();
        static bool noplot;

        static PyObject *matplotlib;

#ifdef USE_VTK
        static double viewport[4];
        static vtkSmartPointer<vtkRenderWindow> renderWindow;
        static vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
        static vtkSmartPointer<vtkChartXY> chart;
        static vtkSmartPointer<vtkRenderer> textRenderer;
        static int ncolor;
#endif

    public:
        static void init(bool noplot = false);
        static void subplot(int, bool clear_axis = false);
        static void plot(const matrix&, std::string label = "", std::string style = "");
        static void plot(const matrix&, const matrix&, std::string label = "", std::string style = "");
        static void semilogx(const matrix&, std::string label = "");
        static void semilogx(const matrix&, const matrix&, std::string label = "");
        static void semilogy(const matrix&, std::string label = "");
        static void semilogy(const matrix&, const matrix&, std::string label = "");
        static void loglog(const matrix&, const matrix&, std::string label = "");
        static void pcolormesh(const matrix&, const matrix&, const matrix&);
        static void axvline(double);
        static void axhline(double);
        static void text(double, double, std::string);
        static void show(bool block = false);
        static void ion();
        static void ioff();
        static void clf();
        static void draw();
        static void legend(std::string = "");
        static void colorbar();
        static void close();
        static void savefig(const std::string&);
        static void title(const std::string&);
        static void pause(double = 1e-4);
        static void figure(const int&, int width=-1, int height=-1);
        static void axis(const double&, const double&, const double&, const double&);
        static void axis(const std::string&);
};

#endif
