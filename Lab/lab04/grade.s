# read a mark and print the corresponding UNSW grade

main:
    la   $a0, prompt    # printf("Enter a mark: ");
    li   $v0, 4
    syscall

    li   $v0, 5         # scanf("%d", mark);
    syscall

    blt  $v0, 50, fail
    blt  $v0, 65, pass
    blt  $v0, 75, credit
    blt  $v0, 85, distinct
    bge  $v0, 85, high_d

    j    end

fail:
    la   $a0, fl        # printf("FL\n");
    li   $v0, 4
    syscall
    j    end 

pass:
    la   $a0, ps        # printf("PS\n");
    li   $v0, 4
    syscall
    j    end

credit:
    la   $a0, cr        # printf("CR\n");
    li   $v0, 4
    syscall
    j    end

distinct:
    la   $a0, dn        # printf("DN\n");
    li   $v0, 4
    syscall
    j    end

high_d:
    la   $a0, hd        # printf("HD\n");
    li   $v0, 4
    syscall
    j    end

end:
    li   $v0, 0
    jr   $ra            # return

    .data
prompt:
    .asciiz "Enter a mark: "
fl:
    .asciiz "FL\n"
ps:
    .asciiz "PS\n"
cr:
    .asciiz "CR\n"
dn:
    .asciiz "DN\n"
hd:
    .asciiz "HD\n"
