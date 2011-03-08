typedef struct {
  tracking_t cam1_red;
  tracking_t cam1_green;
  tracking_t cam2_red;
  tracking_t cam2_green;
} tolerance_file_t;

tolerance_file_t * get_tolerance (char * filename);
