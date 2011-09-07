__kernel void
vadd(__global int * a, __global int * b, __global int * c)
{
    size_t i =  get_global_id(0);
    c[i] = a[i] + b[i];
}
/*
//Only copying the image
__kernel void
sobel_rgb(read_only image2d_t src, write_only image2d_t dst){
    int x = (int)get_global_id(0);
    int y = (int)get_global_id(1);
    
//Verify the image boundaries
    if( x >= get_image_width(src) || y >= get_image_heigth(src))
        return; 

    // Read current pixel
    float4 p00 = read_imagef(src, sampler, (int2)(x,y));
    write_imagef(dst, (int2)(x, y), p00);
}*/
