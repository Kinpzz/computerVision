% read the original image
originalImage = imread('../images/Fig0220(a).tif');
figure(1)
imshow(originalImage)
title('original image')

resizedImage = resizeImage_bilinear(originalImage, 1/12.5);
figure(2)
imshow(resizedImage)
title('resized image')

recovered = resizeImage_bilinear(resizedImage, 12.5);
figure(3)
imshow(recovered)
title('recovered image')