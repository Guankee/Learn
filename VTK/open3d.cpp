#include<iostream>
#include<open3d/Open3D.h>
#include<open3d/core/CUDAUtils.h>
int main(){
  int num=open3d::core::cuda::DeviceCount();
  std::cout<<num<<std::endl;

  return 0;
}