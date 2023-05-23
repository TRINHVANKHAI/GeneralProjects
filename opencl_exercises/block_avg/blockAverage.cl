__kernel void blockAverageGElements ( __global ushort *input,
    __global ushort *output,
    int image_width,
    int image_height,
    int block_width,
    int block_height,
    int bl_offset
)
{
    int col = get_global_id (0);
    int row = get_global_id (1);
    int width = get_global_size(0);
    int bl_px_off;
    int x,y;
    int sum=0;
    for (y=0; y<block_height; y++) {
        bl_px_off = (y * block_height) * image_width + (col * block_width);
        sum += input[bl_px_off+0] + input[bl_px_off+2] + input[bl_px_off+4];
    }
    output[row * width + col] = (2* sum) / (block_width*block_height);
}

