function [resizedImage] = resizeImage_replication(originalImage,scaleFactor)
% Zooming and Shrinking Images by Pixel Replication
if scaleFactor <= 0
    error('Error. ScaleFactor must positive')
end
% size of original image
[row, col] = size(originalImage);
resizedImage = [];
for i = 1 : (row*scaleFactor)
    for j= 1 : (col*scaleFactor)
        x = round(i/scaleFactor);
        y = round(j/scaleFactor);
        if x == 0
            x = 1;
        end
        if y == 0
            y = 1;
        end
        resizedImage(i,j)=originalImage(x,y);
    end
end
% matlab's function imshow support double and uint8,
% while double range is 0~1 so we change it to uint whose range is 0~255
resizedImage = uint8(resizedImage);
end

