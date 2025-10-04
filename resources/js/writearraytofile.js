
function writeTwoDArrayToFile(arrays, filename) {
            const rows = arrays.length;
            const cols = arrays[0].length;
            
            // Create file content
            let content = `${rows} ${cols}\n`;
            for (let i = 0; i < rows; i++) {
                content += arrays[i].join(' ') + '\n';
            }
            
            // Create a blob and download link
            const blob = new Blob([content], { type: 'text/plain' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = filename;
            a.click();
            URL.revokeObjectURL(url);
            
            return content;
        }

