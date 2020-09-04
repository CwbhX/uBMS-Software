import React, { Component } from 'react';
import './app.css';

import io from "socket.io-client";
const ENDPOINT = "192.168.1.131";

import { makeStyles } from '@material-ui/core/styles';
import TextField from '@material-ui/core/TextField';
import Button from '@material-ui/core/Button';
import Switch from '@material-ui/core/Switch';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import IconButton from '@material-ui/core/IconButton';
import MenuIcon from '@material-ui/icons/Menu';


export default class App extends Component {
  state = { username: null,
            dataFromServer: "",
            ip: "",
            connected: "Connect",
            checkedA: false
            };

  ws = ""

  onFormSubmit = (event) => {
      event.preventDefault();
      console.log(this.state.ip);
      if(this.state.connected === "Connect"){
          this.ws = new WebSocket("ws://" + this.state.ip);

          this.ws.onopen = () => {
          // on connecting, do nothing but log it to the console
            console.log('connected')
            this.setState({connected:"Disconnect"});
          }

          this.ws.onmessage = evt => {
          // listen to data sent from the websocket server
          const message = JSON.parse(evt.data)
          this.setState({dataFromServer: message})
            console.log(`Got data!: ${message}`)
          }

          this.ws.onclose = () => {
          console.log('disconnected')
          this.setState({connected: "Connect"});
          // automatically try to reconnect on connection loss
        }
    }else{
        this.ws.close();
        this.setState({connected: "Connect"});
    }
  }

  onTextChange = (event) => {
      console.log(`Event from ${event.target.name} with value: ${event.target.value}`);
      this.setState({ip:event.target.value});
  }

  handleChange = (event) => {
      this.setState({[event.target.name]:event.target.checked});
      this.ws.send(event.target.checked);
  }

  componentDidMount() {
    fetch('/api/getUsername')
      .then(res => res.json())
      .then(user => this.setState({ username: user.username }));
  }

  render() {
    const { username } = this.state;

    return (
      <div>
          <AppBar position="static">
          <Toolbar>
            <IconButton edge="start" color="inherit" aria-label="menu">
              <MenuIcon />
            </IconButton>
            <Typography variant="h6">
              News
            </Typography>
            <Button color="inherit">Login</Button>
          </Toolbar>
        </AppBar>
        {username ? <h1>{`Hello ${username}`}</h1> : <h1>Loading.. please wait!</h1>}
        <form noValidate autoComplete="off" onSubmit={this.onFormSubmit}>
            <TextField id="outlined-basic" label="Enter ESP IP" variant="outlined" name="IP" onChange={this.onTextChange} />
        </form>
        <Button variant="contained" color="primary" onClick={this.onFormSubmit}>
          {this.state.connected}
        </Button>
        <Switch
              checked={this.state.checkedA}
              onChange={this.handleChange}
              name="checkedA"
              inputProps={{ 'aria-label': 'secondary checkbox' }}
        />
      </div>
    );
  }
}
