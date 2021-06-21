/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2021                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#ifndef __OPENSPACE_MODULE_AUTONAVIGATION___HELPERFUNCTIONS___H__
#define __OPENSPACE_MODULE_AUTONAVIGATION___HELPERFUNCTIONS___H__

#include <ghoul/glm.h>
#include <ghoul/logging/logmanager.h>
#include <ghoul/misc/assert.h>
#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

namespace openspace::pathnavigation::helpers {

    // Make interpolator parameter t [0,1] progress only inside a subinterval
    double shiftAndScale(double t, double newStart, double newEnd);

    glm::dquat lookAtQuaternion(glm::dvec3 eye, glm::dvec3 center, glm::dvec3 up);

    glm::dvec3 viewDirection(const glm::dquat& q);

    bool lineSphereIntersection(glm::dvec3 linePoint1, glm::dvec3 linePoint2,
        glm::dvec3 sphereCenter, double spehereRadius, glm::dvec3& intersectionPoint);

    bool isPointInsideSphere(const glm::dvec3& p, const glm::dvec3& c, double r);

    double simpsonsRule(double t0, double t1, int n, std::function<double(double)> f);

    double fivePointGaussianQuadrature(double t0, double t1,
        std::function<double(double)> f);
} // namespace openspace::pathnavigation::helpers

namespace openspace::pathnavigation::interpolation {

    glm::dquat easedSlerp(const glm::dquat q1, const glm::dquat q2, double t);

    // TODO: make all these into template functions.
    // Alternatively, add cubicBezier interpolation in ghoul and only use
    // ghoul's interpolator methods

    // Centripetal version alpha = 0, uniform for alpha = 0.5 and chordal for alpha = 1
    glm::dvec3 catmullRom(double t, const glm::dvec3& p0, const glm::dvec3& p1,
        const glm::dvec3& p2, const glm::dvec3& p3, double alpha = 0.5);

    glm::dvec3 cubicBezier(double t, const glm::dvec3& cp1, const glm::dvec3& cp2,
        const glm::dvec3& cp3, const glm::dvec3& cp4);

    glm::dvec3 linear(double t, const glm::dvec3& cp1, const glm::dvec3& cp2);

    glm::dvec3 hermite(double t, const glm::dvec3 &cp1, const glm::dvec3 &cp2,
        const glm::dvec3 &tangent1, const glm::dvec3 &tangent2);

    glm::dvec3 piecewiseCubicBezier(double t, const std::vector<glm::dvec3>& points,
        const std::vector<double>& tKnots);

    glm::dvec3 piecewiseLinear(double t, const std::vector<glm::dvec3>& points,
        const std::vector<double>& tKnots);

} // namespace openspace::pathnavigation::interpolation 
#endif // __OPENSPACE_MODULE_AUTONAVIGATION___HELPERFUNCTIONS___H__
