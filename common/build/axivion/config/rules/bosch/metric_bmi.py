#  Axivion Bauhaus Suite
#  http://www.axivion.com/
#  Copyright (C) Axivion GmbH, 2019

'''This file provides the metric BMI according to customer's specification.'''

from __future__ import absolute_import

import sys
import math
from . import bosch_metric_base
from bauhaus import rfg
from bauhaus.rfg.misc import create_node_attr_if_missing


class BMIMetric(bosch_metric_base.BoschMetric):
    rulename = bosch_metric_base.BoschMetric._subgroup + 'BMI'

    rfg_metric_name = 'Metric.Bosch.BMI'
    '''Name of the node attribute storing the metric value in the RFG.'''
    display_name = 'Bosch Maintainability Index'
    '''Description of the metric shown in the dashboard.'''
    min_value = None
    '''Minimal allowed value. None if unlimited.'''
    max_value = None
    '''Maximal allowed value. None if unlimited.'''

    _STST3_name = 'Metric.Lines.Code'
    _STMIF_name = 'Metric.Maximum_Extended_Nesting'
    _logSTPTH_name = 'Metric.NPath'

    ruletext = display_name + '.'

    @classmethod
    def get_rulehtml(cls):
        return '''{header}
                  {description}
               '''.format(
            header=cls.get_rulehtml_header(),
            description='''
                        {display_name}<br>
                        <p>BMI is defined as</p>
                        <quote>BMI = 93.06 - 0.11 * STST3 - 0.13 * STMIF - 4.31 * log(STPTH, 2)</quote>
                        <p>With the following meaning</p>
                        <ul>
                        <li> STST3 - number of statements ({STST3_name}) </li>
                        <li> STMIF - maximum nesting of control structures ({STMIF_name}) </li>
                        <li> STPTH - estimated static program paths (STPTH_name) </li>
                        </ul>
                    '''.format(
                display_name=cls.display_name,
                STST3_name=cls._STST3_name,
                STMIF_name=cls._STMIF_name,
                STPTH_name=cls._logSTPTH_name,
            ),
        )

    @classmethod
    def get_rfg_node_attribute(cls, rfg_graph, attr_name):
        try:
            return rfg_graph.node_attribute(attr_name)
        except rfg.InvalidAttribute:
            sys.stderr.write(
                'RFG does not have node attribute {attr} - skipping metric {metric}\n'.format(
                    attr=attr_name, metric=cls.get_full_rulename()
                )
            )
            return None

    @classmethod
    def execute(cls, rfg_graph):
        if not rfg_graph:
            return
        create_node_attr_if_missing(rfg_graph, cls.rfg_metric_name, attr_type='float')
        STST3_attr = cls.get_rfg_node_attribute(rfg_graph, cls._STST3_name)
        STMIF_attr = cls.get_rfg_node_attribute(rfg_graph, cls._STMIF_name)
        logSTPTH_attr = cls.get_rfg_node_attribute(rfg_graph, cls._logSTPTH_name)
        if not STST3_attr or not STMIF_attr or not logSTPTH_attr:
            cls.propagate = False
            return

        base_view = cls.get_base_view(rfg_graph)
        for rfg_node in base_view.xnodes():
            if (
                STST3_attr in rfg_node
                and STMIF_attr in rfg_node
                and logSTPTH_attr in rfg_node
            ):
                STST3 = rfg_node[STST3_attr]
                STMIF = rfg_node[STMIF_attr]
                logSTPTH = rfg_node[logSTPTH_attr]

                BMI = 93.06 - 0.11 * STST3 - 0.13 * STMIF - 4.31 * math.log(logSTPTH,2)

                cls.add_metric_value(rfg_node=rfg_node, metric_value=BMI)
