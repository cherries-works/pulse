/**
 * Parses a given graph element and data.
 * @param {Element} element - The graphical element.
 * @param {string} name - The name of the metric.
 * @param {object} data - The data (array) that has the values.
 */
const parser = (
    element,
    name, 
    data
) => {
    const c = element.querySelector("canvas");
    if (c) c.remove();

    const size = 500

    const canvas = document.createElement("canvas");
    canvas.style.width  = "100%";
    canvas.style.height = "100%";
    canvas.width = size;
    canvas.height = size;

    element.append(canvas);
    const ctx = canvas.getContext("2d");
    ctx.strokeStyle = "white"
    ctx.lineJoin = "round";
    ctx.lineCap = "round";
    ctx.lineWidth = 2;

    if (typeof data != "object") return;
    if (data.length == undefined) return;
    if (data.length == 0) return;

    const clientX = canvas.width;
    const clientY = canvas.height;

    const defaultX = 10;
    const defaultY = 10;

    const padding = 20;

    // setup
    ctx.fillStyle = "#DB324D"
    ctx.font = "30px monospace";
    ctx.fillText(name, 10, 50);

    const canvasXOffset = defaultX + padding * 2;
    const canvasYOffset = defaultY + padding * 2;

    const yAxisStart = defaultY + canvasYOffset;
    const yAxisEnd = canvas.height - defaultY;
    const yAxisHeight = yAxisEnd - yAxisStart - canvasYOffset + defaultY;

    ctx.moveTo(0 + canvasXOffset, yAxisStart);
    ctx.lineTo(0 + canvasXOffset, yAxisEnd);
    ctx.stroke()

    for(let i = 0; i < 10; i++) {
        const y = (yAxisHeight - yAxisHeight * ((i + 1) / 10)) + yAxisStart;
        ctx.moveTo(0 + canvasXOffset - 10, y);
        ctx.lineTo(0 + canvasXOffset + 10, y);
        ctx.stroke()
    }

    const canvasWidth = canvas.width - canvasXOffset;
    
    ctx.moveTo(defaultX, canvas.height - canvasYOffset);
    ctx.lineTo(canvas.width - defaultX, canvas.height - canvasYOffset);
    ctx.stroke()

    const canvasHeight = canvas.height - canvasYOffset;

    ctx.beginPath();

    // start at the first data point
    for (let i = 0; i < data.length; i++) {
        const x = canvasWidth * (i / (Math.max(data.length, 5))) + canvasXOffset;
        const y = (yAxisHeight - yAxisHeight * (data[i] / 100)) + yAxisStart;

        if (i === 0) {
            ctx.moveTo(x, y);
        } else {
            ctx.lineTo(x, y);
        }
    }
    ctx.stroke();
};

const main = async () => {
    const charts = document.querySelectorAll("cw-chart")
    for(let chart of charts) {
        const dataApi = chart.getAttribute("data");
        const dataName = chart.getAttribute("name");
        const dataMetric = chart.getAttribute("metric");
    
        const fetched = await fetch(dataApi);
        const json = await fetched.json();
        
        let data = json.data;
        data.sort((a,b) => Number(a.timestamp) - Number(b.timestamp))
        data = data.map(a => {
            return a.usage
        })


        parser(chart, dataName, data);
    }
}

async function loop() {
  await main();
  setInterval(async () => {
    await main();
  }, 2000);
}

loop();
