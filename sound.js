   
 


export   class FrequencyAnalyzer {
            constructor(options) {
                this.audioContext = null;
                this.analyser = null;
                this.microphone = null;
                this.dataArray = null;
                this.isRunning = false;
                this.animationFrame = null;
                
              }
          
            
            async start() {
                try {
                    this.clearError();
                    this.status.textContent = 'Requesting microphone access...';
                    
                    // Get microphone access
                    const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
                    
                    // Create audio context
                    this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
                    
                    // Create analyser node
                    this.analyser = this.audioContext.createAnalyser();
                    this.analyser.fftSize = 2048;
                    this.analyser.smoothingTimeConstant = parseFloat(this.smoothingInput.value);
                    
                    // Connect microphone to analyser
                    this.microphone = this.audioContext.createMediaStreamSource(stream);
                    this.microphone.connect(this.analyser);
                    
                    // Create data array for frequency data
                    this.dataArray = new Uint8Array(this.analyser.frequencyBinCount);
                    
                    this.isRunning = true;
                    
                    this.analyze();
                    
                } catch (error) {
                    this.showError('Error accessing microphone: ' + error.message);
                    this.status.textContent = 'Failed to start analysis';
                }
            }
            
            stop() {
                this.isRunning = false;
                
                if (this.animationFrame) {
                    cancelAnimationFrame(this.animationFrame);
                }
                
                if (this.microphone) {
                    this.microphone.disconnect();
                }
                
                if (this.audioContext) {
                    this.audioContext.close();
                }
                
                this.clearError();
            }
            
            analyze(minFreq=300,maxFreq=3000,smoothingInput=.8,sensitivity=1) {
                if (!this.isRunning) return {dbLevel:0, normalizedLevel:0, 
                    minFreq:0, maxFreq:0}
                
                // Update analyser settings
                this.analyser.smoothingTimeConstant = parseFloat(smoothingInput);
                
                // Get frequency data
                this.analyser.getByteFrequencyData(this.dataArray);
                
                
                // Convert frequency to bin indices
                const sampleRate = this.audioContext.sampleRate;
                const binCount = this.analyser.frequencyBinCount;
                const binWidth = sampleRate / (2 * binCount);
                
                const minBin = Math.floor(minFreq / binWidth);
                const maxBin = Math.floor(maxFreq / binWidth);
                
                // Calculate average level in frequency band
                let sum = 0;
                let count = 0;
                
                for (let i = minBin; i <= maxBin && i < this.dataArray.length; i++) {
                    sum += this.dataArray[i];
                    count++;
                }
                
                const averageLevel = count > 0 ? sum / count : 0;
                
                // Convert to decibels (approximate)
                const dbLevel = averageLevel > 0 ? 20 * Math.log10(averageLevel / 255) * sensitivity : -60;
                const normalizedLevel = Math.max(0, Math.min(100, (dbLevel + 60) / 60 * 100));
                
                return {dbLevel:dbLevel, normalizedLevel:normalizedLevel, 
                    minFreq:minFreq, maxFreq:maxFreq};
                
            }
            
        }
        
        