class discreteFunction {
    constructor(jsonPath) {
        this.jsonPath = jsonPath;
        this.yValues = null;
        this.isLoaded = false;
        this.n = 0;
        this.loadPromise = null;
        
        // Start loading the function data immediately
        this.loadPromise = this.loadFunction();
    }
    
    /**
     * Load y-values from JSON file
     * @returns {Promise<void>}
     */
    async loadFunction() {
        try {
            const response = await fetch(this.jsonPath);
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            
            const data = await response.json();
            
            if (!Array.isArray(data)) {
                throw new Error('JSON file must contain a single array of y-values');
            }
            
            if (data.length === 0) {
                throw new Error('Y-values array cannot be empty');
            }
            
            // Validate that all values are numbers
            for (let i = 0; i < data.length; i++) {
                if (typeof data[i] !== 'number') {
                    throw new Error(`Invalid y-value at index ${i}: must be a number`);
                }
            }
            
            this.yValues = [...data]; // Create a copy
            this.n = data.length;
            this.isLoaded = true;
            
        } catch (error) {
            console.error('Failed to load function data:', error);
            this.isLoaded = false;
            throw error;
        }
    }
    
    /**
     * Wait for the function to be loaded
     * @returns {Promise<void>}
     */
    async waitForLoad() {
        if (this.loadPromise) {
            await this.loadPromise;
        }
    }
    
    /**
     * Evaluate the function at x using linear interpolation
     * @param {number} x - Input value, will be clamped to [0, 1]
     * @returns {number} - Interpolated y-value
     */
    evaluate(x) {
        if (!this.isLoaded) {
            throw new Error('Function data not loaded yet. Call waitForLoad() or check isReady() first.');
        }
        
        // Clamp x to [0, 1]
        x = Math.max(0, Math.min(1, x));
        
        // Convert x to the array index space [0, n-1]
        const scaledX = x * (this.n - 1);
        
        // Find the two indices to interpolate between
        const i = Math.floor(scaledX);
        const j = Math.min(i + 1, this.n - 1);
        
        // Handle edge case where x maps exactly to the last point
        if (i === j) {
            return this.yValues[i];
        }
        
        // Linear interpolation
        const t = scaledX - i; // fractional part
        const y1 = this.yValues[i];
        const y2 = this.yValues[j];
        
        return y1 + t * (y2 - y1);
    }
    
    /**
     * Check if the function is ready for evaluation
     * @returns {boolean}
     */
    isReady() {
        return this.isLoaded;
    }
    
    /**
     * Get information about the loaded function
     * @returns {Object|null}
     */
    getInfo() {
        if (!this.isLoaded) return null;
        
        return {
            sampleCount: this.n,
            yValues: [...this.yValues], // Return a copy
            domain: [0, 1],
            range: [Math.min(...this.yValues), Math.max(...this.yValues)]
        };
    }
    
    /**
     * Get the raw y-values array (copy)
     * @returns {number[]|null}
     */
    getYValues() {
        return this.isLoaded ? [...this.yValues] : null;
    }
}

// Example usage:
/*
// Example JSON file content (func.json):
// [0.0, 0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25, 0.0]

// Method 1: Wait for loading explicitly
async function example1() {
    const func = new discreteFunction('localpath/func.json');
    await func.waitForLoad();
    
    if (func.isReady()) {
        console.log('Function info:', func.getInfo());
        const y = func.evaluate(0.3);
        console.log('f(0.3) =', y);
    }
}

// Method 2: Check readiness and wait if needed
async function example2() {
    const func = new discreteFunction('localpath/func.json');
    
    if (!func.isReady()) {
        console.log('Waiting for function to load...');
        await func.waitForLoad();
    }
    
    const y = func.evaluate(0.3);
    console.log('f(0.3) =', y);
}

// Method 3: Polling approach (not recommended but works)
function example3() {
    const func = new discreteFunction('localpath/func.json');
    
    function tryEvaluate() {
        if (func.isReady()) {
            const y = func.evaluate(0.3);
            console.log('f(0.3) =', y);
        } else {
            setTimeout(tryEvaluate, 100); // Try again in 100ms
        }
    }
    
    tryEvaluate();
}

// Example of creating and using multiple functions
async function multipleExample() {
    const func1 = new discreteFunction('path/func1.json');
    const func2 = new discreteFunction('path/func2.json');
    
    // Wait for both to load
    await Promise.all([func1.waitForLoad(), func2.waitForLoad()]);
    
    if (func1.isReady() && func2.isReady()) {
        console.log('f1(0.5) =', func1.evaluate(0.5));
        console.log('f2(0.5) =', func2.evaluate(0.5));
    }
}
*/