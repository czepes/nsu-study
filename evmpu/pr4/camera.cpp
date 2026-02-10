#include <map>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

class Performance {
private:
  inline static const double frequency{getTickFrequency()};
  double capture_time{0};
  double process_time{0};
  double display_time{0};
  int64 start_ticks{0};
  int frames{0};

public:
  void start() {
    printf("\n\n\n\n\n\n\n");
    if (frames == 0) {
      start_ticks = getTickCount();
    }
  }

  void add_capture_time(int64 capture_start) {
    capture_time += (getTickCount() - capture_start) / frequency;
  }
  void add_process_time(int64 process_start) {
    process_time += (getTickCount() - process_start) / frequency;
  }
  void add_display_time(int64 display_start) {
    display_time += (getTickCount() - display_start) / frequency;
  }

  void print() {
    int64 current_ticks = getTickCount() - start_ticks;

    if (current_ticks >= frequency) {
      double elapsed_time = current_ticks / frequency;
      double total_time = capture_time + process_time + display_time;
      double fps = frames / elapsed_time;

      printf("\033[7A"
             "------- Performance -------\n"
             "Capture: %5.1f ms. (%04.1f %%) \n"
             "Process: %5.1f ms. (%04.1f %%) \n"
             "Display: %5.1f ms. (%04.1f %%) \n"
             "Summary: %5.1f ms. (%04.1f %%) \n"
             "Elapsed: %5.1f ms. (100  %%)   \n"
             "FPS:     %5.1f                 \n",
             capture_time / frames * 1000, capture_time / elapsed_time * 100,
             process_time / frames * 1000, process_time / elapsed_time * 100,
             display_time / frames * 1000, display_time / elapsed_time * 100,
             total_time / frames * 1000, total_time / elapsed_time * 100,
             elapsed_time / frames * 1000, fps);

      frames = 0;
      capture_time = process_time = display_time = 0;
      start_ticks = getTickCount();
    }

    frames++;
  }
};

enum Option {
  RED = 0,
  GREEN = 1,
  BLUE = 2,
  MONO = 3,
  SHARP = 4,
  REDUX = 5,
  OPT_AMOUNT = 6
};

class Options {
private:
  inline static const map<char, Option> keys{{'b', BLUE},  {'g', GREEN},
                                             {'r', RED},   {'m', MONO},
                                             {'s', SHARP}, {'x', REDUX}};
  vector<bool> options;

public:
  static void controls() {
    printf("Controls:\n"
           "<B> - toggle blue channel\n"
           "<G> - toggle green channel\n"
           "<R> - toggle red channel\n"
           "<M> - toggle monochrome\n"
           "<S> - toggle sharpening\n"
           "<X> - toggle color reduction\n"
           "<Q> - quit\n");
  }

  Options() {
    options.resize(OPT_AMOUNT);
    options[BLUE] = true;
    options[GREEN] = true;
    options[RED] = true;
  }

  void set(char key) {
    const auto it = keys.find(key);
    if (it == keys.end()) {
      return;
    }
    const Option option = (*it).second;
    options[option] = !options[option];
  }

  bool get(Option option) const { return options[option]; }
  bool operator[](Option option) const { return options[option]; }
};

class ImgProcessor {
private:
  static Mat generate_redux_table() {
    int redux_width{64};
    Mat table(1, 256, CV_8U);
    uchar *ptr{table.ptr()};

    for (int i = 0; i < 256; i++) {
      ptr[i] = redux_width * (i / redux_width);
    }
    return table;
  }

  static Mat generate_kernel() {
    return (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
  }

  inline static const Mat kernel = generate_kernel();
  inline static const Mat redux_table = generate_redux_table();

  static void filter_colors(Mat &src, Mat &dst, Options &options) {
    if (src.empty() || src.depth() != CV_8U || src.channels() != 3) {
      return;
    }

    if (src.size != dst.size || dst.depth() != CV_8U || dst.channels() != 3) {
      return;
    }

    bool blue = options[BLUE];
    bool green = options[GREEN];
    bool red = options[RED];

    if (blue && green && red) {
      return;
    }

    for (int y = 0; y < src.rows; y++) {
      Vec3b *src_row = src.ptr<Vec3b>(y);
      Vec3b *dst_row = dst.ptr<Vec3b>(y);

      for (int x = 0; x < src.cols; x++) {
        dst_row[x][0] = blue ? src_row[x][0] : 0;
        dst_row[x][1] = green ? src_row[x][1] : 0;
        dst_row[x][2] = red ? src_row[x][2] : 0;
      }
    }
  }

public:
  void apply(Mat &src, Mat &dst, Options &options) const {
    if (options[MONO]) {
      cvtColor(src, dst, COLOR_BGR2GRAY);
    } else {
      filter_colors(src, dst, options);
    }

    if (options[REDUX]) {
      LUT(src, redux_table, dst);
    }

    if (options[SHARP]) {
      filter2D(src, dst, src.depth(), kernel);
    }
  }

  void apply(Mat &image, Options &options) const {
    apply(image, image, options);
  }
};

int main(void) {
  Options options;
  ImgProcessor processor;
  Performance monitor;

  VideoCapture capture(0);
  Mat image;

  options.controls();
  monitor.start();

  while (true) {
    int64 capture_start = getTickCount();
    capture.read(image);
    monitor.add_capture_time(capture_start);

    int64 process_start = getTickCount();
    processor.apply(image, options);
    monitor.add_process_time(process_start);

    int64 display_start = getTickCount();
    imshow("Camera", image);
    monitor.add_display_time(display_start);

    monitor.print();

    char c = waitKey(1);
    if (c == 27 || c == 'q') {
      break;
    } else {
      options.set(c);
    }
  }

  capture.release();
  return 0;
}
