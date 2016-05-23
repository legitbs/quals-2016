import sys

def insert_rdx_free( lines ):
  new_lines = []
  stage = 0
  for l in range(len(lines)):
    new_lines.append(lines[l])

    if lines[l].lstrip().rstrip() == 'free:':
      stage = 1

    if lines[l].lstrip().rstrip() == '.cfi_startproc' and stage == 1:
      new_lines.append('\tpushq %rdx\n')

      stage = 2

    if lines[l].lstrip().rstrip() == 'ret' and stage == 2:
      t = new_lines.pop()
      new_lines.append('\tpopq %rdx\n')
      new_lines.append(t)
      stage = 3

  if stage != 3:
    print 'ERROR FAIL!!!!!!'

  return new_lines

def insert_r9_afb( lines ):
  new_lines = []
  stage = 0
  for l in range(len(lines)):
    new_lines.append(lines[l])

    if lines[l].lstrip().rstrip() == 'add_free_block:':
      stage = 1

    if lines[l].lstrip().rstrip() == '.cfi_def_cfa_offset 64' and stage == 1:
      t = new_lines.pop()
      s = new_lines.pop()
      new_lines.append('\tpushq %r9\n')
      new_lines.append(s)
      new_lines.append(t)
      stage = 2

    if lines[l].lstrip().rstrip() == '.cfi_def_cfa_offset 8' and stage == 2:
      new_lines.append('\tpopq %r9\n')
      stage = 3

  if stage != 3:
    print 'ERROR FAIL!!!!!!'

  return new_lines

def insert_r8_dp( lines): 
  new_lines = []
  stage = 0
  for l in range(len(lines)):
    new_lines.append(lines[l])

    if lines[l].lstrip().rstrip() == 'debug_print:':
      stage = 1

    if lines[l].lstrip().rstrip() == '.cfi_def_cfa_offset 32' and stage == 1:
      t = new_lines.pop()
      s = new_lines.pop()
      new_lines.append('\tpushq %r8\n')
      new_lines.append(s)
      new_lines.append(t)
      stage = 2

    if lines[l].lstrip().rstrip() == '.cfi_def_cfa_offset 8' and stage == 2:
      new_lines.append('\tpopq %r8\n')
      stage = 3

  if stage != 3:
    print 'ERROR FAIL!!!!!!'

  return new_lines


def insert_rsi_rd( lines): 
  new_lines = []
  stage = 0

  for l in range(len(lines)):
    new_lines.append(lines[l])

    if lines[l].lstrip().rstrip() == 'read_delim:':
      stage = 1

    if lines[l].lstrip().rstrip() == '.cfi_startproc' and stage == 1:
      new_lines.append('\tpushq %rsi\n')
      stage = 2

    if lines[l].lstrip().rstrip() == 'ret' and stage == 2:
      t = new_lines.pop()
      new_lines.append('\tpopq %rsi\n')
      new_lines.append(t)
      stage = 3

  if stage != 3:
    print 'ERROR FAIL!!!!!!'

  return new_lines

def insert_rdi_rp( lines): 
  new_lines = []
  stage = 0
  for l in range(len(lines)):
    new_lines.append(lines[l])

    if lines[l].lstrip().rstrip() == 'random_pill:':
      stage = 1

    if lines[l].lstrip().rstrip() == '.cfi_startproc' and stage == 1:
      new_lines.append('\tpushq %rdi\n')
      stage = 2

    if lines[l].lstrip().rstrip() == 'ret' and stage == 2:
      t = new_lines.pop()
      new_lines.append('\tpopq %rdi\n')
      new_lines.append(t)
      stage = 3

  if stage != 3:
    print 'ERROR FAIL!!!!!!'

  return new_lines

def main( nm ):
  f = open(nm, 'rb')
  data = f.read()
  f.close()

  lines = data.split('\n')

  if nm == 'malloc.s':
    lines = insert_r9_afb( lines)
    lines = insert_r8_dp(lines)
    lines = insert_rdx_free(lines)

  if nm == 'pillpusher.s':
    lines = insert_rsi_rd(lines)
    lines = insert_rdi_rp( lines )

  x = []

  i = 0
  while i < len(lines):
    if lines[i].find( '.cfi_startproc') != -1:
      x.append(lines[i])
      x.append('\tpushf\n')
    elif lines[i].lstrip().rstrip() == 'ret':
      x.append('\tpopf\n')
      x.append(lines[i])
    else:
      x.append(lines[i])

    i += 1


  f = open(nm, 'wb')
  f.write('\n'.join(x))
  f.close()


main(sys.argv[1])
