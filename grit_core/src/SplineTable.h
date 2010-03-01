/* Copyright (c) David Cunningham and the Grit Game Engine project 2010
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef SplineTable_h
#define SplineTable_h

#include <map>

// http://en.wikipedia.org/wiki/Cubic_Hermite_spline

template<class T> class SplineTable {

    public:

        void addPoint (T x, T y)
        {
                points[x] = y;
        }

        void commit (void)
        {
                // calculate tangents
                if (points.size()==0) return;
                if (points.size()==1) {
                        tangents[points.begin()->first] = 0;
                        return;
                }
                if (points.size()==2) {
                        typename Map::iterator i = points.begin();
                        T lastx = i->first, lasty = i->second;
                        i++;
                        T nextx = i->first, nexty = i->second;
                        tangents[lastx] = tangents[nextx]
                                        = (nexty-lasty)/(nextx-lastx);
                        return;
                }

                {
                        MI i;
                        i = points.begin();
                        T lastx = i->first, lasty = i->second;
                        i++;
                        T nextx = i->first, nexty = i->second;
                        tangents[lastx] = (nexty-lasty)/(nextx-lastx);
                }

                MI next = points.begin(); std::advance(next,2);
                MI curr = points.begin(); std::advance(curr,1);
                MI last = points.begin(); std::advance(last,0);
                for (MI i_=points.end() ; next!=i_  ; next++, curr++, last++ ) {
                        T lastx = last->first, lasty = last->second;
                        T nextx = next->first, nexty = next->second;
                        tangents[curr->first] = (nexty-lasty)/(nextx-lastx);
                }

                {
                        MI i;
                        i = points.end(); std::advance(i,-2);
                        T lastx = i->first, lasty = i->second;
                        i++;
                        T nextx = i->first, nexty = i->second;
                        tangents[nextx] = (nexty-lasty)/(nextx-lastx);
                }

        }

        T maxX (void) {
                MI i = points.end();
                i--;
                return i->first;
        }

        T minX (void) {
                MI i = points.begin();
                return i->first;
        }

        T operator[] (T x) {
                if (points.size()==0) return 0;
                if (points.size()==1) return points.begin()->second;

                {
                        T minx = minX(), maxx = maxX();
                        if (x<=minx) {
                                return points[minx] + (x-minx)*tangents[minx];
                        }
                        if (x>=maxx) {
                                return points[maxx] + (x-maxx)*tangents[maxx];
                        }
                }

                // note that due to the minx early return we do not need
                // to initialise these
                T x0, y0;

                for (MI i=points.begin(), i_=points.end() ; i!=i_ ; ++i) {
                        if (i->first > x) {
                                // i never == points.begin()
                                T x1 = i->first, y1 = i->second;
                                T m0 = tangents[x0], m1 = tangents[x1];
                                T h = x1 - x0;
                                T t = (x - x0)/h;
                                T r = 0;
                                r += (1+2*t)*(1-t)*(1-t  ) * y0;
                                r += (    t)*(1-t)*(1-t  ) * h*m0;
                                r += (    t)*(t  )*(3-2*t) * y1;
                                r += (    t)*(t  )*(t-1  ) * h*m1;
                                return r;
                        }
                        x0 = i->first;
                        y0 = i->second;
                }
                // should never get here
                abort();
                return 0.0;  // MSVC doesn't understand abort()
        }

        typedef std::map<T,T> Map;
        typedef typename Map::iterator MI;

        const Map &getTangents (void) const { return tangents; }
        const Map &getPoints (void) const { return points; }

    protected:

        Map points;
        Map tangents;

};


#endif

// vim: tabstop=8:shiftwidth=8:expandtab
