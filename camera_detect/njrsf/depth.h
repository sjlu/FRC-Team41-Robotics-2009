#define CAMERA_DIST_BETWEEN 7.0
// #define CAMERA_VIEW_FIELD 0.942477796 AXIS206
#define CAMERA_VIEW_FIELD 0.701919065  // logitech quickcam
#define IMAGE_WIDTH 320.0
#define IMAGE_HEIGHT 240.0
#define PI 3.14159265358979323846

double image_angle (double x);

double find_depth (double x, double x2, int method);
