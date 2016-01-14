!=================================================================
!  QWML module for writing quantum walk probability distributions
!  to qwml files compatible with qwViz.
!
!  Developed by Scott D. Berry for release with qwViz 1.0
!  Modified: 4.3.11
!================================================================
!
!  write_qwml requires: 
!  filename: a character string with the filename, including extension.
!  adjacency: an n x n integer array with the adjacency matrix.
!  probDist: a t x n array of probabilities for each vertex in the graph.
!  graphName: (non-essential) character string (pass 'none' for unnamed graph).
!  comment: (non-essential) character string (pass 'none' for unnamed graph).
!  ios: holds the iostat values associated with opening the file.
module QWML
implicit none
!
contains 
  subroutine write_qwml(filename, adjacency, probDist, graphName, comment, ios)
    character(len=*), intent(in)         :: filename
    integer, dimension(:,:), intent(in)  :: adjacency
    real(8), dimension(:,:), intent(in)  :: probDist
    character(len=*), intent(in)         :: graphName
    character(len=*), intent(in)         :: comment
    integer                              :: ios
    !  local variables
    integer                              :: n, steps, colNum, rowNum, t, vertex
    character(len=80)                    :: str
    !
    !  read the number of vertices and steps from incoming data size
    n = size(adjacency(1,:))
    steps = size(probDist(:,1))
    !
    open(1, file=trim(adjustl(filename)), status='replace', &
         iostat=ios)
    write(1,'(a)') '<?xml version="1">'
    write(1,'(a)') '<qwml>'
    write(1,'(a,1x)') '<adjacency>'
    do rowNum = 1, n
       write(str,'(i8)') rowNum
       write(1,'(a)') '<row>'
       do colNum = 1, n
          write(str,'(i8)') colNum
          write(1,'(a,i1,a)') '<col>', adjacency(rowNum,colNum),'</col>'
       end do
       write(1,'(a)') '</row>'
    end do
    write(1,'(a,1x)') '</adjacency>'
    write(1,'(a,1x)') '<probdist>'
    do vertex = 1, n
       write(str,'(i8)') vertex
       write(1,'(a)') '<vertex>'
       do t = 1, steps
          write(1,'(a,f10.8,a)')'<prob>',probDist(t,vertex),'</prob>'
       end do
       write(1,'(a)') '</vertex>'
    end do
    write(1,'(a)') '</probdist>'
    write(1,'(a,a,a)') '<filename>', trim(adjustl(filename)), '</filename>'
    write(1,'(a,a,a)') '<graphname>', trim(adjustl(graphName)), '</graphname>'
    write(1,'(a,a,a)') '<comment>', trim(adjustl(comment)), '</comment>'
    write(1,'(a)') '</qwml>'
    close(1)
    !
  end subroutine write_qwml
end module QWML
