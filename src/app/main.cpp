#include "App.h"
int main(int argc, char *argv[]) {
  app()->init();
  app()->start();
  app()->exec();
  return 0;
}
