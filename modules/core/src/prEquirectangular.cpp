#include <per/prEquirectangular.h>

prEquirectangular::prEquirectangular(double au,double av,double u0,double v0, double k1,double k2,double k3,double k4,double k5):prCameraModel(au,av,u0,v0, k1, k2, k3, k4, k5)
{
    type = Equirectangular;
    name = "Equirectangular";
}


void prEquirectangular::init(double _au,double _av,double _u0,double _v0, double _k1,double _k2,double _k3,double _k4,double _k5)
{
    prCameraModel::init(_au,_av,_u0,_v0,_k1,_k2,_k3, _k4, _k5);
    
    type = Equirectangular;
    name = "Equirectangular";
}

void prEquirectangular::project3DImage(prPointFeature & P)
{
    double X = P.get_X(), Y = P.get_Y(), Z = P.get_Z();
   
    // P.set_x(atan2(Y,X)); //Azimuth entre -pi et pi
    // P.set_y(atan2(Z, sqrt(X*X + Y*Y))); //Elevation entre -pi/2 et pi/2
    P.set_x(atan2(X, Z));                   // Azimuth entre -pi et pi
    P.set_y(atan2(Y, sqrt(X * X + Z * Z))); // Elevation entre -pi/2 et pi/2
}

bool prEquirectangular::unProject(prPointFeature & P, double & Depth)
{
    double Xs, Ys, Zs;
    
    if(!projectImageSphere(P, Xs, Ys, Zs))
        return false;

    P.set_X(Xs*Depth);
    P.set_Y(Ys*Depth);
    P.set_Z(Zs*Depth);
    
    return true;
}

void prEquirectangular::project3DSphere(prPointFeature & P, double & Xs, double & Ys, double & Zs)
{
    double inv_norme;
    
    Xs = P.get_X();
    Ys = P.get_Y();
    Zs = P.get_Z();
    
    inv_norme = 1.0 / sqrt(Xs*Xs + Ys*Ys + Zs*Zs);
    
    Xs *= inv_norme;
    Ys *= inv_norme;
    Zs *= inv_norme;
}

bool prEquirectangular::projectImageSphere(prPointFeature & P, double & Xs, double & Ys, double & Zs)
{
    double x = P.get_x(), y = P.get_y();
    
    Zs = cos(y) * cos(x);
    Xs = cos(y) * sin(x);
    Ys = sin(y);
    
    return true;
}

prEquirectangular& prEquirectangular::operator=(const prEquirectangular& cam)
{
    prCameraModel::operator=(cam);

    return *this;
}

// du / dX
void prEquirectangular::computeSensorJacobian(prPointFeature & P, vpMatrix & LuX)
{
    double X = P.get_X(), Y = P.get_Y(), Z = P.get_Z();
    double X2=X*X, Y2=Y*Y, Z2 = Z*Z;
    double X2pZ2 = X2+Z2;
     
    if(X2pZ2 < 1e-8)
    {
        LuX.resize(2,3,true);
    }
    else
    {
        //get du / dx from mother class
        vpMatrix Lux(2,2);
        prCameraModel::computeSensorJacobian(P, Lux);

        double srX2pZ2 = sqrt(X2pZ2);
        double X2pY2pZ2 = X2pZ2+Y2;

        vpMatrix LxX(2,3);
        LxX[0][0] = Z/X2pZ2;                 LxX[0][1] = 0.;               LxX[0][2] = -X/X2pZ2;
        LxX[1][0] = -X*Y/(srX2pZ2*X2pY2pZ2); LxX[1][1] = srX2pZ2/X2pY2pZ2; LxX[1][2] = -Y*Z/(srX2pZ2*X2pY2pZ2);

        LuX = Lux*LxX;
    }
}
