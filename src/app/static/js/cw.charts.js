/**
 * Parses a given graph element and data.
 * @param {Element} element - The graphical element.
 * @param {object} data - The data (array) that has the values.
 */
const parser = (element, data) => {
    element.style.width = "500px";
    element.style.height = "500px";

    const canvas = document.createElement("canvas")
    const ctx = canvas.getContext("2d");
    element.append(canvas);

    if(typeof(data) != "object") return;
    if(data.length == undefined) return;
    if(data.length == 0) return;

    // 100% on the Y axis
    const clientY = element.clientHeight;
    
    // 100% on the X axis
    const clientX = element.clientWidth;

    // const maxX = _max(data);
    ctx.lineWidth = 10;

    const defaultX = 10;
    const defaultY = 10;

    ctx.beginPath();
    ctx.moveTo(defaultX, defaultY);
    for(let i = 0; i < data.length; i++) {
        const currentX = (clientX / data.length) * i;
        const currentY = defaultY + data[i];
        ctx.lineTo(currentX, currentY);
    }
    ctx.stroke();
}

setTimeout(async () => {
    const charts = document.querySelectorAll("cw-chart")
    for(let chart of charts) {
        const dataApi = chart.getAttribute("data");
        const dataMetric = chart.getAttribute("metric");
    
        const fetched = await fetch(dataApi);
        const json = await fetched.json();
        const data = json.data;
    
        parser(chart, data);
    }
})
