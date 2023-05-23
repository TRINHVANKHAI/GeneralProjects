__kernel void helloworld ( __global ushort *input,
    __global ushort *output,
    int image_width,
    int image_height,
    int block_width,
    int block_height
)
{
    int col = get_global_id (0);
    int row = get_global_id (1);
    int size = 128;
    int bl_global_id_x = col * block_width;
    int bl_global_id_y = row * block_height;
    
    output[row*size+col] = row*size+col;
}

