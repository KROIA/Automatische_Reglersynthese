function filePaths = findFilesByName(folderPath, fileName)
% FINDFILESBYNAME Recursively finds all files with a specific name
%
%   filePaths = findFilesByName(folderPath, fileName)
%
% Inputs:
%   folderPath - parent folder to search
%   fileName   - name of the file to find (e.g., 'data.csv')
%
% Output:
%   filePaths  - cell array of full paths to matching files

    % Use MATLAB's recursive dir function (requires R2016b or newer)
    files = dir(fullfile(folderPath, '**', fileName));
    
    % Filter out directories just in case
    files = files(~[files.isdir]);
    
    % Collect full paths
    filePaths = fullfile({files.folder}, {files.name});
end


