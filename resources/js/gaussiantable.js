
// Constants
const TABLE_SIZE = 128;
const INPUT_RANGE = 1.0;  // Total range is [0, 1] = 1.0
const CENTER = 0.5;       // Distribution centered at 0.5

/**
 * Initialize the Gaussian lookup table with specified variance
 * @param {number} variance The variance (σ²) of the Gaussian distribution
 */
function initGaussianTable(variance) {
    let GAUSSIAN_TABLE = new Array(TABLE_SIZE)
     const sigma = Math.sqrt(variance);
    const normFactor = 1.0 / Math.sqrt(2.0 * Math.PI * variance);
    
    for (let i = 0; i < TABLE_SIZE; i++) {
        // Map table index to x value in [0, 1]
        const x = i / (TABLE_SIZE - 1);
        
        // Calculate distance from center (0.5)
        const distFromCenter = x - CENTER;
        var pow = -(distFromCenter * distFromCenter) / (2.0 * variance)
        // Calculate Gaussian height: (1/(σ√(2π))) * exp(-(x-μ)²/(2σ²))
        GAUSSIAN_TABLE[i] = normFactor * Math.exp(pow);
    }
    return GAUSSIAN_TABLE

}

/**
 * Get Gaussian distribution height for a value in [0, 1]
 * @param {number} x Input value in range [0, 1]
 * @return {number} Gaussian height from precomputed table
 */
function gaussianHeight(x, GAUSSIAN_TABLE) {
    // Clamp input to valid range
    if (x < 0) x = 0;
    if (x > 1) x = 1;
    
    // Map x from [0, 1] to table index [0, 127]
    const index = Math.round(x * (TABLE_SIZE - 1));
    
    // Ensure index is in bounds (safety check)
    const clampedIndex = Math.max(0, Math.min(TABLE_SIZE - 1, index));
    
    return GAUSSIAN_TABLE[clampedIndex];
}

let GAUSSIAN_TABLES={}

function initGaussianTables(){
    GAUSSIAN_TABLES[".001"]=initGaussianTable(.001)
    GAUSSIAN_TABLES[".01"]=initGaussianTable(.01)
    GAUSSIAN_TABLES[".02"]=initGaussianTable(.02)
    GAUSSIAN_TABLES[".04"]=initGaussianTable(.04)
}





// Example usage and testing
function testGaussianLookup() {
    // Initialize table with variance = 0.04 (sigma = 0.2)
    const variance = 0.04;
    initGaussianTable(variance);
    
    console.log(`Gaussian heights for variance = ${variance}:`);
    console.log(`x = 0.0: ${gaussianHeight(0.0).toFixed(6)}`);
    console.log(`x = 0.25: ${gaussianHeight(0.25).toFixed(6)}`);
    console.log(`x = 0.5: ${gaussianHeight(0.5).toFixed(6)}`);    // Peak at center
    console.log(`x = 0.75: ${gaussianHeight(0.75).toFixed(6)}`);
    console.log(`x = 1.0: ${gaussianHeight(1.0).toFixed(6)}`);
    
    console.log('\nWith interpolation:');
    console.log(`x = 0.123: ${gaussianHeightInterpolated(0.123).toFixed(6)}`);
    console.log(`x = 0.456: ${gaussianHeightInterpolated(0.456).toFixed(6)}`);
    console.log(`x = 0.789: ${gaussianHeightInterpolated(0.789).toFixed(6)}`);
}
