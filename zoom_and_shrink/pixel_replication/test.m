% read the original image
originalImage = imread('../images/Fig0220(a).tif');
figure(1)
imshow(originalImage)
title('original image')

resizedImage = resizeImage_replication(originalImage, 1/10);
figure(2)
imshow(resizedImage)
title('resized image')

recovered = resizeImage_replication(resizedImage, 10);
figure(3)
imshow(recovered)
title('recovered image')