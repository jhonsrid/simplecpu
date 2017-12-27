; Rebase test
MOV $I10 112233

MOV R10 11111111
REBASE 30
MOV R10 33333333
REBASE 40
MOV R10 44444444
REBASE 50
MOV R10 55555555

; Should NOT be able to move backwards! Ie this should FAIL at assembly time
;REBASE 20
;MOV R10 20202020
