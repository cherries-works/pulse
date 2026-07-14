const system_status = document.querySelector("#system_status")

const cpu_percentage = document.querySelector("#cpu_percentage")
const cpu_temp = document.querySelector("#cpu_temp")
const cpu_idle = document.querySelector("#cpu_idle")
const cpu_total = document.querySelector("#cpu_total")
const cpu_processes = document.querySelector("#cpu_processes")

const memory_percentage = document.querySelector("#memory_percentage")
const memory_usage = document.querySelector("#memory_usage")
const memory_total = document.querySelector("#memory_total")
const memory_available = document.querySelector("#memory_available")

const disk_percentage = document.querySelector("#disk_percentage")
const disk_usage = document.querySelector("#disk_usage")
const disk_total = document.querySelector("#disk_total")
const disk_available = document.querySelector("#disk_available")

const network_rx = document.querySelector("#network_rx")
const network_tx = document.querySelector("#network_tx")

const io_read = document.querySelector("#io_read")
const io_write = document.querySelector("#io_write")

const load_1 = document.querySelector("#load_1")
const load_5 = document.querySelector("#load_5")
const load_15 = document.querySelector("#load_15")

const uptime = document.querySelector("#uptime")

const processes = document.querySelector(".processes")
const createProcess = (process, _cpu) => {
    const p = document.createElement("div")
    p.className = "process"

    const pid = document.createElement("p")
    pid.textContent = `(${process.pid})`

    const cpu = document.createElement("p")
    cpu.textContent = `${((process.cpu / _cpu.total) * 100).toFixed(2)}%`

    const ram = document.createElement("p")
    ram.textContent = `${(process.ram / 1024).toFixed(2)}MB`

    const name = document.createElement("p")
    name.textContent = process.name

    p.append(pid)
    p.append(cpu)
    p.append(ram)
    p.append(name)
    
    return p
}


let previousSnapshot = null;
const updateSystem = async () => {
    const f = await fetch("/api/metrics")
    const json = await f.json()
    if(!json.success) return

    const _cpu = json.cpu
    const _memory = json.memory
    const _disk = json.disk
    const _network = json.network
    const _load = json.load
    const _processes = json.processes
    const _temp = json.temp

    const parsed_cpu_usage = parseCpuUsage(_cpu, previousSnapshot?.cpu);
    cpu_temp.textContent = `(${_temp / 1000}°C)`
    cpu_percentage.textContent = `${(parsed_cpu_usage).toFixed(2)}%`
    cpu_idle.textContent = Intl.NumberFormat("de").format(_cpu.idle)
    cpu_total.textContent = Intl.NumberFormat("de").format(_cpu.total)
    cpu_processes.textContent = Intl.NumberFormat("de").format(_cpu.processes)

    const parsed_memory_usage = ((_memory.total - _memory.available) / _memory.total) * 100;
    memory_percentage.textContent = `${(parsed_memory_usage).toFixed(2)}%`
    memory_available.textContent = `${(_memory.available / 1024 / 1024).toFixed(2)}GB`
    memory_total.textContent = `${(_memory.total / 1024 / 1024).toFixed(2)}GB`
    memory_usage.textContent = `${((_memory.total - _memory.available) / 1024 / 1024).toFixed(2)}GB`

    const parsed_disk_usage = ((_disk.total - _disk.available) / _disk.total) * 100;
    disk_percentage.textContent = `${(parsed_disk_usage).toFixed(2)}%`
    disk_available.textContent = `${(_disk.available / 1024 / 1024 / 1024).toFixed(2)}GB`
    disk_total.textContent = `${(_disk.total / 1024 / 1024 / 1024).toFixed(2)}GB`
    disk_usage.textContent = `${((_disk.total - _disk.available) / 1024 / 1024 / 1024).toFixed(2)}GB`

    let network_usage = parseNetworkUsage(_network, previousSnapshot?.network)
    network_rx.textContent = `${(network_usage.rx / 1024).toFixed(2)}KB/s`
    network_tx.textContent = `${(network_usage.tx / 1024).toFixed(2)}KB/s`

    let io_usage = parseIoUsage(_disk, previousSnapshot?.disk)
    io_write.textContent = `${((io_usage.r * 512) / 1024).toFixed(2)}KB/s`
    io_read.textContent = `${((io_usage.w * 512) / 1024).toFixed(2)}KB/s`

    uptime.textContent = formatTimeHumanReadable(json.uptime)
    
    load_1.textContent = _load.load1
    load_5.textContent = _load.load5
    load_15.textContent = _load.load15

    processes.innerHTML = ""
    for(let process of _processes) {
        processes.append(createProcess(process, _cpu))
    }

    previousSnapshot = json

    const usage = parsed_cpu_usage + parsed_disk_usage + parsed_memory_usage
    if(usage < 125) {
        system_status.textContent = "HEALTHY"
    } else if(usage < 200) {
        system_status.textContent = "WARNING"
    } else {
        system_status.textContent = "DANGER"
    }
}

const parseNetworkUsage = (snapshot2, snapshot1) => {
    let rx = snapshot2.rx - (snapshot1?.rx ?? 0);
    let tx = snapshot2.tx - (snapshot1?.tx ?? 0);

    let n = {
        rx,
        tx
    };
    
    return n;
}

const parseIoUsage = (snapshot2, snapshot1) => {
    let r = snapshot2.reads - (snapshot1?.reads ?? 0);
    let w = snapshot2.writes - (snapshot1?.writes ?? 0);

    let io = {
        r,
        w
    };
    
    return io;
}

const parseCpuUsage = (snapshot2, snapshot1) => {
    let total_time_float = snapshot2.total - (snapshot1?.total ?? 0);
    let idle_all_time_float = snapshot2.idle - (snapshot1?.idle ?? 0);

    let diff_total_idle = total_time_float - idle_all_time_float;
    let cpu_usage = diff_total_idle / total_time_float;
    cpu_usage *= 100;

    return cpu_usage;
}


const formatTimeHumanReadable = (seconds) => {
    let days = seconds / 3600 / 24;
    days = Math.round(days)

    let hours = seconds / 3600;
    hours = Math.round(hours)

    let minutes = (seconds % 3600) / 60;
    minutes = Math.round(minutes)

    let secs = seconds % 60 % 60 % 60;
    secs = Math.round(secs)

    if(days > 0) {
        return `${days}d ${hours}h ${minutes}m ${secs}s`
    } else if(hours > 0) {
        return `${hours}h ${minutes}m ${secs}s`
    } else {
        return `${minutes}m ${secs}s`
    }
}


setInterval(async () => {
    await updateSystem()
}, 1000)
