_pulse() {
    local cur 
    cur="${COMP_WORDS[COMP_CWORD]}"
    local command 
    command="${COMP_WORDS[1]}"

    local options 
    local word

    # if there is not a command set yet print the commands first
    if [[ $COMP_CWORD -eq 1 ]]; then
        local commands="monitor info top stop config prune help snapshot process version"
        COMPREPLY=( $(compgen -W "$commands" -- "$cur") )
        return
    fi

    case "$command" in
        monitor)
            options="--port --web --sleep --headless --processes --sort"
            ;;
        info|snapshot)
            options="--json"
            ;;
        top)
            options="--processes --sort"
            ;;
        process)
            options="--process"
            ;;
        prune)
            options="--keep --until --prune"
            ;;
        config)
            options="--reset"
            ;;
        *)
            return
            ;;
    esac

    for word in "${COMP_WORDS[@]:2}"; do
        # if the argument is already passed in, then dont re-suggest it
        case "$word" in
            --port|--web|--sleep|--headless|--processes|--sort|\
            --json|--process|--keep|--until|--prune|--reset)
                options="${options//$word/}"
                ;;
        esac
    done

    COMPREPLY=($(compgen -W "$options" -- "$cur"))
}

complete -F _pulse pulse