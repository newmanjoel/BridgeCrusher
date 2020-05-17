# -*- coding: utf-8 -*-

import threading
import logging
import math
import time
import datetime

import dash
import dash_core_components as dcc
import dash_daq as daq
import dash_html_components as html
import chart_studio.plotly as py
from dash.dependencies import Input, Output
#from HMIBox import HMI as Hmi
from Hardware_interface.HMIBox import HMI as Hmi

#import plotly.plotly as py
#import chart_studio.plotly.graph_objs as go
data = {
    'x1': [],
    'y1': []
}
total_length = 5000
time_length = 20
add_more_data = True
start_time = time.time()
message = 'No Message to display2'



external_stylesheets = ['https://codepen.io/chriddyp/pen/bWLwgP.css']

app = dash.Dash(__name__, external_stylesheets=external_stylesheets)

app.layout = html.Div([
        dcc.Tabs(children=[
            dcc.Tab(style={'marginLeft':'1.5em'},label='Live Results',children=[
                html.H1(children='Bridge Crusher'),
            
                html.Div(children='''
                    The Graph showing the pressure over time for the bridge
                '''),
            
                dcc.Input(id='my-id', value='name of team', type='text'),
            
                html.Div(id='output-1'),
                html.Div(id='output-2'),
            
                html.Button('Save Data', id='save-button'),
                html.Button('Pause Data', id='pause-button'),
            
                dcc.Graph(
                    id='live-update-graph'
                ),
                daq.Gauge(
                    id='current-weight',
                    showCurrentValue=True,
                    units="Kg",
                    value=0,
                    label='Live Value',
                    max=1023,
                    min=0,
                ),
                dcc.Interval(
                        id='interval-component',
                        interval=100,  # in milliseconds
                        n_intervals=0
                ),
                dcc.Interval(
                        id='interval-component-slow',
                        interval=500,  # in milliseconds
                        n_intervals=0
                ),
                html.Div(style={'marginLeft':'1.5em'}, children=[
                    daq.Slider(
                        id='history-slider',
                        min=1,
                        max=5000,
                        step=1,
                        value=100,
                        handleLabel={"showCurrentValue": True,"label": "VALUE"}
                    ),
                ]),
                html.H5(id='message_out', children='No Message to display'),
                html.H5(id='max_value', children=0)
            ]),
            dcc.Tab(label='Settings',children=[
                html.Div(id='Setting-Tab',children=[
                    html.H6(children='This is on the settings page'),
                    daq.BooleanSwitch(
                      id='background_switch',
                      on=True
                    )
                ])
            ])
        ])
])

#@app.callback(Output('current-weight', 'color'),
#              [Input('max_value', 'children'),
#               Input('interval-component-slow', 'n_intervals')])
#def background_test(n):
#    return {"gradient":True,"ranges":{"green":[0,n],"red":[n,1023]}}
    


@app.callback(Output('current-weight', 'value'),
              [Input('interval-component', 'n_intervals')])
def update_live(n):
    hmi = Hmi()
    data_point = hmi.async_results;
    analog_inputs = data_point['AI']
    return analog_inputs[0]

@app.callback(Output('message_out', 'children'),
              [Input('interval-component-slow', 'n_intervals')])
def message_out(n):
    global message
    return message


@app.callback(Output('live-update-graph', 'figure'),
              [Input('interval-component-slow', 'n_intervals'),
               Input('history-slider', 'value'),
               Input('max_value','children')])
def update_graph_live(n, length,max_value):
    hmi = Hmi()
    xdata = []
    ydata = []
    N = min([len(hmi.history), length])
    local_history = hmi.history[-N:]
    for i in range(0, len(local_history)):
        data_point = local_history[i]
        analog_inputs = data_point['AI']
        ydata.append(analog_inputs[0])
        xdata.append(datetime.datetime.fromtimestamp(data_point['LastUpdate']))


    #logging.getLogger(__name__).warning("point:{}, history:{}".format(i,ydata))
    #delta_time = max(data['x1'])-min(data['x1'])
    #time_left = delta_time
    #time_right = delta_time + 1
    #x_data = data['x1'][-1500:]
    #y_data = data['y1'][-1500:]
    
    #fig.update(data=[{'y': y_data, 'x': x_data}], layout={'title': len(data['x1']), 'xaxis': {'range': [time_left, time_right]}})
    title = "Length: {}".format(len(xdata))
    return_data = {'data': [{'y': ydata, 'x': xdata}], 'layout': {'title': title }}
    global message
    message = "Max Value:{}".format(max(ydata))
    return return_data;


@app.callback(Output('output-1', 'children'),
              [Input('pause-button', 'n_clicks')])
def update_graph_live(n):
    global add_more_data
    add_more_data = not add_more_data



def start_web_server():
    app.run_server(debug=False)


def start_data_collection():
    global data, message
    n = 0
    while True:
        time.sleep(0.01)  # Every 10 ms
        message = "n is {} in the data collection".format(n)
        if(add_more_data):
            message += " and adding more!"
            data['y1'].append(math.sin(math.radians(n)))
            data['x1'].append(n)
            if len(data['x1']) > total_length:
                data['x1'] = data['x1'][-total_length:]
                data['y1'] = data['y1'][-total_length:]
            n += 1

if __name__ == '__main__':
    try:
        #t1 = threading.Thread(target=start_data_collection)
        t2 = threading.Thread(target=start_web_server)
        #t1.start()
        t2.start()
    except:
        print ("error: unable to start a thread")


