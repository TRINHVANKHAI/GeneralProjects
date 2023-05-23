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
    int id;
    int bl_col = col * block_width;
    int bl_row = row * block_height;
    int x ;
    int temp=0;
    for (id=0;id<block_height;id++) {
        x = (bl_row+id) * image_width + bl_col;
        temp += input[x+0] + input[x+2] + input[x+4];
    }
    output[row*block_width+col] = temp/(3*block_height);
}

