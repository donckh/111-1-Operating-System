#!/usr/bin/env bash
set -eou pipefail
IFS=$'\n\t'

trap cleanup ERR EXIT SIGINT SIGTERM

BOLD="$(tput bold)"
RED="$(tput setaf 1)"
GREEN="$(tput setaf 2)"
RESET="$(tput sgr0)"


# You can add your own testcases here
testcases=("-n 1 -t 0.5 -s NORMAL -p -1"
           "-n 2 -t 0.5 -s FIFO,FIFO -p 10,20"
           "-n 3 -t 1.0 -s NORMAL,FIFO,FIFO -p -1,10,30")


pre_run() {
    # Save the origin value of rt_runtime
    ori_runtime=$(sysctl kernel.sched_rt_runtime_us | cut -d '=' -f 2 | tr -d ' ')

    # Let the the value of rt_period equal to the value of rt_runtime
    local period
    period=$(sysctl kernel.sched_rt_period_us | cut -d '=' -f 2 | tr -d ' ')
    sysctl -w "kernel.sched_rt_runtime_us=$period" > /dev/null 2>&1
}

cleanup() {
    # Restore the origin value of rt_runtime   
    sysctl -w "kernel.sched_rt_runtime_us=${ori_runtime}" > /dev/null 2>&1
}

run_tests() {
    failed() {
        printf "Result: %s%sFailed...%s\n" "$RED" "$BOLD" "$RESET" 
        exit 1
    }
    success() {
        printf "Result: %s%sSuccess!%s\n" "$GREEN" "$BOLD" "$RESET" 
    }

    for i in ${!testcases[@]}; do
        printf "Running testcase %d: ./sched_demo %s ......\n" "$((i+1))" "${testcases[$i]}"
        if diff --color=always <(eval "${ta_program} ${testcases[$i]}") \
                               <(eval "${your_program} ${testcases[$i]}");
        then
            success
        else
            failed
        fi
    done
}

usage() {
    printf "Usage: %s <ta's-program> <your-program> \n" "$0" 
    exit 1
}

parse_args() {
    if [[ $# -ne 2 ]]; then
        usage "$0"
    fi

    ta_program="$1"
    your_program="$2"
}

main() {
    parse_args "$@"
    pre_run
    run_tests
    cleanup
}

main "$@"
