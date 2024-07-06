import string
from unittest.mock import Mock

import lldb
import unittest
import re

FIXED_TYPE_PATTERN = r'fixed<(\s*\d{1,3})\s*,\s*(\d+)'


def fixed_point_summary(valobj: lldb.SBValue, _):
    fixed_point_typename = valobj.GetType().GetCanonicalType().GetName()

    print(fixed_point_typename)

    # looks like fixed<size, scaling_factor_inverse ...>
    typename_arg_matcher = re.search(FIXED_TYPE_PATTERN, fixed_point_typename)
    scaling_factor_inverse = int(typename_arg_matcher.group(2))

    fp_representation = valobj.GetChildMemberWithName('data').GetValueAsSigned()

    return str(fp_representation / scaling_factor_inverse)

def __lldb_init_module(debugger: lldb.SBDebugger, _):
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed64_low')
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed32_low')
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed16_low')
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed64_mid')
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed32_mid')
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed16_mid')
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed64_high')
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed32_high')
    debugger.HandleCommand('type summary add -F fixed_point_lldb_format.fixed_point_summary fixed16_high')


class FixedPointSummaryTest(unittest.TestCase):
    def testFixedPointSummary(self):
        def generate_test_data(data: int, typename: string):
            sb_value = Mock(spec=lldb.SBValue)

            mock_type = Mock()
            canonical_mock_type = Mock()
            canonical_mock_type.GetName.return_value = typename

            mock_type.GetCanonicalType.return_value = canonical_mock_type
            sb_value.GetType.return_value = mock_type
            mock_data = Mock()
            mock_data.GetValueAsSigned.return_value = data
            sb_value.GetChildMemberWithName.return_value = mock_data
            return sb_value

        test_data = {
            (5632, 'fixed<16, 128, void, short>'): '44.0',
            (1081344, 'fixed<64, 32768, void, long long>'): '33.0',
            (-281018368, 'fixed<32, 8388608, void, int>'): '-33.5'
        }

        for key, value in test_data.items():
            self.assertEqual(fixed_point_summary(generate_test_data(*key), None), value)
