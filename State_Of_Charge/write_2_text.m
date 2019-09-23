function out = write_2_text(input)
%This function first converts the doubles into hex values then prints the
%look-up table data into the text file in a code-friendly format
load('matlab');
count = 0;
fileID = fopen('LUT_C.txt', 'w');
fprintf(fileID, 'SoC LUT Data\n\n')
for i = length(xBLKAbv0)
    if count < 6
        fprintf(fileID, '%f, ', xBLKAbv0);
        count = count + 1;
    else 
        fprintf(fileID, '\n');
        count = 0;
    end

fclose(fileID); 
    
end



end 
