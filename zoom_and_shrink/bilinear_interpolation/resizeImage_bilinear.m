function [resizedImage] = resizeImage_bilinear(originalImage,scaleFactor)
%Zooming and Shrinking Images by Bilinear Interpolation
if scaleFactor <= 0
    error('Error. ScaleFactor must positive')
end

[row, col]=size(originalImage);
originalImage = [originalImage; zeros(1,col)]; % add a col zeros
originalImage = [originalImage zeros(row+1, 1)]; % add a row zeros

ini_u = (row-1)/(scaleFactor*row-1); % 每一步相当于原图的比例
ini_v = (col-1)/(scaleFactor*col-1);

for i = 1:scaleFactor*row
    z_u = floor((i-1)*ini_u+1); % 向左取整数
    u = (i-1)*ini_u+1 - z_u; % 小数部分
    for j=1:scaleFactor*col
        z_v = floor((j-1)*ini_v+1);
        v = (j-1)*ini_v+1 - z_v;
        %==== bilinear interpolation formula====
        resizedImage(i,j) = (1-u) * (1-v) * originalImage(z_u, z_v) + ...
                            (1-u) * v * originalImage(z_u, z_v + 1) + ...
                            u * (1-v) * originalImage(z_u + 1, z_v) + ...
                            u * v * originalImage(z_u + 1, z_v+1);
    end
end
resizedImage = uint8(resizedImage);
end

