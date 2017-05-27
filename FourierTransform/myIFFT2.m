function [ f ] = myIFFT2( F )

    f = conj(myFFT2(conj(F)))/(size(F,1)*size(F,2));
end