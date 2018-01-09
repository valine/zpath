#!/bin/sh

# Display usage
cpack_usage()
{
  cat <<EOF
Usage: $0 [options]
Options: [defaults in brackets after descriptions]
  --help            print this message
  --prefix=dir      directory in which to install
  --include-subdir  include the Zpath-0.1.0-Linux subdirectory
  --exclude-subdir  exclude the Zpath-0.1.0-Linux subdirectory
EOF
  exit 1
}

cpack_echo_exit()
{
  echo $1
  exit 1
}

# Display version
cpack_version()
{
  echo "Zpath Installer Version: 0.1.0, Copyright (c) Humanity"
}

# Helper function to fix windows paths.
cpack_fix_slashes ()
{
  echo "$1" | sed 's/\\/\//g'
}

interactive=TRUE
cpack_skip_license=FALSE
cpack_include_subdir=""
for a in "$@"; do
  if echo $a | grep "^--prefix=" > /dev/null 2> /dev/null; then
    cpack_prefix_dir=`echo $a | sed "s/^--prefix=//"`
    cpack_prefix_dir=`cpack_fix_slashes "${cpack_prefix_dir}"`
  fi
  if echo $a | grep "^--help" > /dev/null 2> /dev/null; then
    cpack_usage 
  fi
  if echo $a | grep "^--version" > /dev/null 2> /dev/null; then
    cpack_version 
    exit 2
  fi
  if echo $a | grep "^--include-subdir" > /dev/null 2> /dev/null; then
    cpack_include_subdir=TRUE
  fi
  if echo $a | grep "^--exclude-subdir" > /dev/null 2> /dev/null; then
    cpack_include_subdir=FALSE
  fi
  if echo $a | grep "^--skip-license" > /dev/null 2> /dev/null; then
    cpack_skip_license=TRUE
  fi
done

if [ "x${cpack_include_subdir}x" != "xx" -o "x${cpack_skip_license}x" = "xTRUEx" ]
then
  interactive=FALSE
fi

cpack_version
echo "This is a self-extracting archive."
toplevel="`pwd`"
if [ "x${cpack_prefix_dir}x" != "xx" ]
then
  toplevel="${cpack_prefix_dir}"
fi

echo "The archive will be extracted to: ${toplevel}"

if [ "x${interactive}x" = "xTRUEx" ]
then
  echo ""
  echo "If you want to stop extracting, please press <ctrl-C>."

  if [ "x${cpack_skip_license}x" != "xTRUEx" ]
  then
    more << '____cpack__here_doc____'
LICENSE
=======

This is an installer created using CPack (https://cmake.org). No license provided.


____cpack__here_doc____
    echo
    echo "Do you accept the license? [yN]: "
    read line leftover
    case ${line} in
      y* | Y*)
        cpack_license_accepted=TRUE;;
      *)
        echo "License not accepted. Exiting ..."
        exit 1;;
    esac
  fi

  if [ "x${cpack_include_subdir}x" = "xx" ]
  then
    echo "By default the Zpath will be installed in:"
    echo "  \"${toplevel}/Zpath-0.1.0-Linux\""
    echo "Do you want to include the subdirectory Zpath-0.1.0-Linux?"
    echo "Saying no will install in: \"${toplevel}\" [Yn]: "
    read line leftover
    cpack_include_subdir=TRUE
    case ${line} in
      n* | N*)
        cpack_include_subdir=FALSE
    esac
  fi
fi

if [ "x${cpack_include_subdir}x" = "xTRUEx" ]
then
  toplevel="${toplevel}/Zpath-0.1.0-Linux"
  mkdir -p "${toplevel}"
fi
echo
echo "Using target directory: ${toplevel}"
echo "Extracting, please wait..."
echo ""

# take the archive portion of this file and pipe it to tar
# the NUMERIC parameter in this command should be one more
# than the number of lines in this header file
# there are tails which don't understand the "-n" argument, e.g. on SunOS
# OTOH there are tails which complain when not using the "-n" argument (e.g. GNU)
# so at first try to tail some file to see if tail fails if used with "-n"
# if so, don't use "-n"
use_new_tail_syntax="-n"
tail $use_new_tail_syntax +1 "$0" > /dev/null 2> /dev/null || use_new_tail_syntax=""

tail $use_new_tail_syntax +147 "$0" | gunzip | (cd "${toplevel}" && tar xf -) || cpack_echo_exit "Problem unpacking the Zpath-0.1.0-Linux"

echo "Unpacking finished successfully"

exit 0
#-----------------------------------------------------------
#      Start of TAR.GZ file
#-----------------------------------------------------------;

� �&TZ �\tTՙ�'�L(����3�G�*3I&�@�)I�e��d�Lf�̛��eł�Ӕ6R�R�i�i�u{�l��Kٞ�*�h7���՞R;Q�ƪ]u���w澗��Z��=�{�����}���ޟ{�������t;]e͡Hbp�DVar�\k�T�@�k�\<�$�]Q^^�*��p��.wU��R�>J'�R"�b�J8�+���˧��п��ݤ�cr<����G0���>P���(���?K���Ϯ��h����+�E����n�U�vW�����4�?��7��+����`4����۹��?���?��#*�����5�J���Z���J������;|�V�%�[��.��C�Y�P#̂�E�j!�N�#x4t��f4����g�_�U�k�]J��p4GS��~\�R�顯��T�X�N�ۦ��v���ꝣz�<�"��QV?;��S��֋�z*W��cj��t���崼����ݢ����
�?9(�B�3k?��Q��á����᮲0�5�k�˪+�񨳜�䠲��;�<kG��<OP� �ܳ!��l�q�:�����䛫�Ő�	*�m���b��8�hm��ut�p�P*����d�xh~��0�]���~���U��W|s��_�>�u�xi]j)�>\��۲$��;����?�}�C�������۷-��M/���g���;}g�ӻf՝�<�ad�i�#��_\��s&\��g���+}�h.o,���y�����W�?sw�M��}ꅯ?U�i����s�}e�d���k�=y�~`e�莎5�6�x>o����o�Шs�U&�^�.���1����L�k"��D�)|��/0���`��g5��2��1��
쎡M?1�����o"_c�n-&v�a"�Ȥ}�lR�&v
���&�6��I�&vV���b���K��g��B��O��NRn�`Y�!�F�\��A�Hx�j��~%�g��x��ʵ�#v�7.<�\��C���}}�/-���I�����{��m�:�,D]�Gi��t��	�_�Bm�z����R�tv�*~^��J��u�Ϧ��ӕ{�����Y����Q�nV�?é�U�-����;��C8J��l�a���e�����Z?��=:��sp|No��h��vXK�l��y���MK�r��b#�o����k�֋=�ߥ�N��x����w㓼B���[�.9&���ko�G#r{�3,����H���H� m��P��p �㪺�2�	)�mw��6��-��*/+�䡺@8��"P��4�}r'k�QM��q%�9��&H��E��,�ym��D����(:�����@� ��p�7 �ªcu1,s��}_,�i|m	���zP�K�b���m�،-솜�X`�7aua�Fz�[C���h���n&�Nw�zX޽U��y���B����Ls�z�_m�;^@����W���eU�b`#��@jW�� ��բ�����F�=��F9�z��i�E{��(����K��"8L�j�&bBS��d=숄�������pq�+��Oa�U������.I
:Cn���q*)C���tG} ��-��ջ}�QjBѸ��˕ؐ*.�J�P�h$)CҀK^C̻��r�an�B���`��v{1I�BJ
 ���ܾ�M�-��*`��j@�q��%y0(��mM}�3�)���im�\�7CZ���|�xC�1u�J����0��>3���U���`�A�˱���(ݐ�r�V�A��.u|uB�H�1n�+�q�{��CT;����3����:"����jo�pV%����V*O��	i�n �ƺ:�����j�rg��J�޶�J��Y���[�$�᮫MV�=��w���/�6�?'#�K�3���f!_a�Z����VV�&��;we������k�fc9s�%����C����L�M8c�>w�����[sT����X��Q���ە�Q�����	���Z�=_Q��sx5��rx-�o�p����Opx/��q���s�i_���9�����[/��I���zg�rp>�Wr���7px	�_��.��k�D�rx)��8���v��p~�����z����8����9|���wr�,��p~]���gs� ����}~����>��E~��~�Ë9�!��?���8�����1_��9|!��sx	����E~��sx���u�I_��S~5��,����9|9�;8\����)��/���s��X�w�)9�OY|,�����:Y����E�N�^�<Yi�8������㔚'��ǩ41F�=���x��1�q�L%�m�㔙!|'�8U&�~;���D?� �Scb'�7"�C����㐟�~�8�'\�/G���H�U��Оp���㐞�y���!?y�?��/&�'������{�#�'���G�O����O�O��_@�O��@~!�?�F��ԟ�?A~�?�E~1�?�#��ԟ����RR���I�	�M䗑��~䗓����a�ER�w��B�Hf��Ҕ�٭�����&[ۛ��/]�']�<�������E��p�f������S���Ł%��Ȅ�X��f�σ�b�)��NƏz����I�|���I�����q���S�g��f֊����}7�����PLE����<��5R҉�2tl���y�ˠ�1�**��wL�����H�}ǟLL�����U`ğ|�襖A���9E�Er�T2�z�c2�r�>�Kݹ�u|:%��{ɒ(���Fqj���7��u=]�a��R��r����6�N:�
��]cE��S
N�zj �Q��'��UH��Fѡ6dO�%��;^.����@'�e(dҿ蘿(J�
�U��1g�79��Rɔ���Z�e/����c$7瞅�4�:�'�<=�% �;����k�u?� �qa�F&�_A��{��� ����g����G3���3�_nj���9,�h�S5k4�g��9�9*=�P����?�ٷ�m&%?G�}��s��}J���X�ݐA/ j�h}M���h���;ir��y�e<���ÛW�w���'����ω)j��;�OF�I��TS�֎-kG���Wf����x���*�S��2þ��E�{�h�ذ�I�<��_+���kEžK�O�D�g�G�QI�ʣo�0�w��Co�q3kA�1��'�,:��~+;����
�obk:� ����$*�o����ކ�L)�Lx{KݎF����|�CO��z��h+�ԿIݘ� ď�������M�ǘ����ߤ��6'�#y[s�H&���}ǒ��νS�4�����gSo��R�Ȟ���T�[���(ʪS�{x;��=�xo�n�&��o{�n����h[S�rS��+/�w����R�<��Et?�4'_jN^��y������׾�x	_�z����^��H���N�w��+�&Y4o�<���E'�z�I��}8l��a��YMO��.b_@�[���/�嚄"�r�B���u`�8���/Ć��[�F��`�P��K�+��@���@:����([Y��׋.��z��t�\O�� �/�ﻌ�X�����B�0+ ̑��u�x�����
艀%�Vi�¡����YuX'vskb�.B��@�:E��C7�2�ܥ���J����)p\,���=bl�#o��=�q�(L�q�� �����{�'�@�:�GG�nz�i�l��o˞-�e�aY2'/Ē�@_��P[s7�W�eQ>�z:M���%�f���'lX�~U�'�N>����`�|{�uW���h>6���i>w6gT1ԙ��|�W����f�a��>l���[w_\s��~�'c�������K��r7�~e����@�|>�U��:^O��^Cm��K��/�|��!{ma�r�������y-��Xa����-,�-A�k�I{+`g����%�6Gv^'̤�4�f�L�I3i&ͤ�4��R��P=��Jy�,&T�h�Fw�w�]˯�<��b���^;��t�9b�7������i�Ͼ���北H�XU{l�Q����M�ٞ�*���؞���)�I���7R	�`߫l/��i���h����#����!+���Z�ʧ)������E�ϙ�9v}:H��>J�M�#�>N�YJ_��2���\�J�����J�R�M� �)���oS���Sz��)�Li.=����k)����ҭ�vS:@�A��C�<�T�<��uu��Ҏ�DDI�U�J���:A8�ߺ���Jg�J�]�jW�k��]>�`��8-n��hq��������9�y��s3�K���K�z6����̼�����ŧ��U�9B�!~�0b�?6�~nWų�m�xq&�B��jD#�c.M��e�Z|~澧���B�yj��;��e�kZ|��S�~YŗN�ԓC�����7�|X�_�u�����s�Iq���_"��<
{�����������9H��˃&��'"���q�yNR���s�����Ζ\$�b��;o�.�kt�B���}�vh5�\�����O�s�7R�sg�	�9h֏�Y0o�0��X��W0��0i����E�o�n��|�8��׏ϋ���i�~�����eE;���r����M�q����ߛM�L�	���'MΏ4����8���q|�C&v�������y�7L�̱�C����������Q�q<ǿ،�.�ĝ��3��ȵ�1,2�G)5��&��7�����?6�c�m"�Y<�	�5�8~�a�ڍ�l�2�o��h7��x�$ÕclǓc_�n"�`�q��	���'s��^���\0)���x��\�x���v֛�͹�q*���!�ع+�8��۹��:?0������ey[�q̂<cyW�q܏�D~�	��3��	��<㸫��-	��W��Π I�H�Mm�$d�j$�O
bh�LtE��p�3���h,.�B0����˹����X��9�$!�%6$tc��ԕ��ð	�#��5�������6�ٽ�?$�ݡ�,���K5�F�BQ�����9������f7��A)N�A$�~A��%��A9�w��;�R��΄Z7	^�Kʠ�nWcs�ͻI�L�T�%<�e�������."]R��z����o��6�J�Sj��R�o�O�m���%�/�qs���S�l�xT���T�W��-Mu����e�4Mڥh� u㦆�6_���m�I���X-aq4-f�82JU���0	?�x���0"m��Iԟ��!�[<]��I��&j�}�
��ez܌Q���R���l��#W� Z7Yb����Ú4Mb���a�gR7=,�Æ����H"m]��X+�QaT���1���aN:'���q�N�a\��Fq8=�H<g*i�f0>5qy��0���B5�aP eh�Ŵi+���2֜P6��5���a�$��8 P����)�N�JL��슌�~��*���L	�P�'�pv&B᮲P�@��@�WpvE��J���O�8�3�� /&�(H���
	/�=Q� ['iIg,J�"N����vŲX��KRH�8�1���-#�BAR�V��(��ʂ�|���p�H�YSS��Ĩ�~�ʢ�1v��E���u�v���uV������q��r:e�l=�ޡ8���c�0mԽ��>]�=B���F����
��g��ar����߭��W���:�4��:��:�KP�>�֋E� �O��o�����=��|���}�����,揭?�Џ�߃����(d�v&���J��%A���I�>[���������{u�l�{�nH��K�M����R}��9���-�>[/y�U�������v����1�?_WQ��}�~��!��-:y}�'t�l����#݀׷��>[��������ϳ������_I����/�w�����z��2cy=���������?%d�&��߿Ry6~��`��^Gt�����W.�Q�E��Y��A��I�e�Wt�l}G��R4)�%<���u�O��������ˎ�W�p�~}1W6��>�.��_?���˧A�?�(��^�~��eR�Nt��<-�/&ͤ�4�f����ߢ�z` ^  