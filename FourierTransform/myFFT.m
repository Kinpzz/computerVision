
function [ y ] = myFFT(x,W)
    N = length(x);
    s = log2(N);
    
    offset=1;
    x=bitrevorder(x);
    %in place
    for stage = 1:s
        for index=0:(2^stage):(N-1)
            for n=0:offset-1
                pos=n+index+1;
                pow=(2^(s-stage))*n+1;
                x1 = x(pos);
                x2 = x(pos+offset)*W(pow);
                x(pos) = x1 + x2;
                x(pos+offset)=x1-x2;
            end
        end
        offset = 2*offset;
    end
    y=x;
end